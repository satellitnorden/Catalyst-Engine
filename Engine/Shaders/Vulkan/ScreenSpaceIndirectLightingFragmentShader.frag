//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRandomUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define SCREEN_SPACE_INDIRECT_LIGHTING_SAMPLES (16)
#define SCREEN_SPACE_INDIRECT_LIGHTING_RANDOM_SAMPLE_WEIGHT (0.9f) //0.025f step.

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_3_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	The probability density function.	
*/
float ProbabilityDensityFunction(vec3 normal, vec3 ray_direction)
{
	return max(dot(normal, ray_direction), 0.0f);
}

/*
*	Calculates the indirect lighting ray direction and start offset.
*/
void CalculateIndirectLightingRayDirectionAndStartOffset(vec3 view_direction, vec3 normal, float roughness, float metallic, out vec3 ray_direction, out float start_offset)
{
	//Calculate the noise texture coordinate.
	vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);

	//Sample the noise texture.
	vec4 noise_texture_sample = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate);

	//Calculate the random rotation matrix.
	mat3 random_rotation = CalculateGramSchmidtRotationMatrix(normal, mix(vec3(1.0f, 1.0f, 1.0f), noise_texture_sample.xyz * 2.0f - 1.0f, SCREEN_SPACE_INDIRECT_LIGHTING_RANDOM_SAMPLE_WEIGHT));

	//Calculate the random hemisphere sample start index.
	uint random_hemisphere_sample_start_index = uint(noise_texture_sample.w * 64.0f * SCREEN_SPACE_INDIRECT_LIGHTING_RANDOM_SAMPLE_WEIGHT);

	//Sample the random direction and length.
	vec3 random_hemisphere_direction;
	float random_length;

	SampleHammersleyHemisphereSample(random_hemisphere_sample_start_index + uint(gl_FragCoord.x) + uint(gl_FragCoord.y), random_hemisphere_direction, random_length);

	//Rotate the random direction.
	random_hemisphere_direction = random_rotation * random_hemisphere_direction;

	//Flip the direction, if needed.
	random_hemisphere_direction = dot(random_hemisphere_direction, normal) >= 0.0f ? random_hemisphere_direction : -random_hemisphere_direction;

	//Calculate the reflection direction.
	vec3 reflection_direction = reflect(view_direction, normal);

	//Blend the random hemisphere direction and the reflection direction based on the material properties.
	ray_direction = normalize(mix(reflection_direction, random_hemisphere_direction, roughness * (1.0f - metallic)));

	//Write the start offset.
	start_offset = random_length;
}

/*
*	Casts a ray against the scene. Returns if there was a hit.	
*/
bool CastRayScene(vec3 ray_origin, vec3 ray_direction, out vec3 hit_radiance)
{
	//Perform the raycast.
	for (uint i = 0; i < SCREEN_SPACE_INDIRECT_LIGHTING_SAMPLES; ++i)
	{
		//Calculate the sample position.
		vec3 sample_position = ray_origin + ray_direction * float(i);

		//Calculate the sample screen coordinate.
		vec4 sample_view_space_position = viewMatrix * vec4(sample_position, 1.0f);
		float sample_screen_coordinate_inverse_denominator = 1.0f / sample_view_space_position.w;
		vec2 sample_screen_coordinate = sample_view_space_position.xy * sample_screen_coordinate_inverse_denominator * 0.5f + 0.5f;

		//If the sample screen coordinate goes outisde of the screen, there can't be a hit.
		if (!ValidCoordinate(sample_screen_coordinate))
		{
			return false;
		}

		//Calculate the expected view distance.
		float expected_view_distance = CalculateViewSpacePosition(sample_screen_coordinate, sample_view_space_position.z * sample_screen_coordinate_inverse_denominator).z;

		//Sample the sample scene features.
		vec4 sample_scene_features = texture(scene_features_2_texture, sample_screen_coordinate);
		float sample_view_distance = CalculateViewSpacePosition(sample_screen_coordinate, sample_scene_features.w).z;

		//If the expected hit distance is greater then the sample hit distance, there is a hit!
		if (expected_view_distance < sample_view_distance)
		{
			//Test the normal of the hit surface against the ray direction - if not within the hemisphere, then there is no hit.
			if (dot(sample_scene_features.xyz, ray_direction) <= 0.0f)
			{
				//Test the direction to the hit position against the ray direction - they must match somewhat for this to be considered a valid hit.
				vec3 sample_world_position = CalculateWorldPosition(sample_screen_coordinate, sample_scene_features.w);

				vec3 direction_to_hit_position = normalize(sample_world_position - ray_origin);

				if (dot(ray_direction, direction_to_hit_position) >= 0.0f)
				{
					//Sample the scene at the sample screen coordinate.
					hit_radiance = texture(scene_texture, sample_screen_coordinate).rgb;

					//Return that there was a hit.
					return true;
				}
			}
		}
	}

	//There was no hit.
	return false;
}

void main()
{
	//Sample the scene features.
	vec4 scene_features_2 = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec4 scene_features_3 = texture(scene_features_3_texture, fragment_texture_coordinate);

	//Unpack the scene features.
	vec3 normal = scene_features_2.xyz;
	float depth = scene_features_2.w;
	float roughness = scene_features_3.x;
	float metallic = scene_features_3.y;

	//Calculate the world position at this fragment.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, depth);

	//Calculate the view direction.
	vec3 view_direction = normalize(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the ray direction and start offset.
	vec3 ray_direction;
	float start_offset;

	CalculateIndirectLightingRayDirectionAndStartOffset(view_direction, normal, roughness, metallic, ray_direction, start_offset);

	//Calculate the indirect lighting.
	vec3 indirect_lighting;

	bool hit = CastRayScene(world_position + ray_direction * start_offset, ray_direction, indirect_lighting);

	//Calculate the probability density.
	float probability_density = ProbabilityDensityFunction(normal, ray_direction);

    //Write the fragment
    fragment = vec4(hit ? indirect_lighting : vec3(0.0f), (hit ? 1.0f : 0.0f) * probability_density);
}