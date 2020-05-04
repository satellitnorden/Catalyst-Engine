//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Constants.
#define SCREEN_SPACE_INDIRECT_LIGHTING_SAMPLES (1)
#define SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES (16)
#define SCREEN_SPACE_INDIRECT_LIGHTING_RAY_STEP (1.0f / SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_features_3_texture;
layout (set = 1, binding = 3) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	The probability density function.	
*/
float ProbabilityDensityFunction(vec3 normal, vec3 ray_direction, float roughness, float metallic)
{
	return mix(1.0f, max(dot(normal, ray_direction), 0.0f), roughness * (1.0f - metallic));
}

/*
*	Calculates the indirect lighting ray direction and start offset.
*/
void CalculateIndirectLightingRayDirectionAndStartOffset(uint index, vec3 view_direction, vec3 normal, float roughness, float metallic, out vec3 ray_direction, out float start_offset)
{
	//Sample the noise texture.
	vec4 noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

	//Calculate the random rotation matrix.
	mat3 random_rotation = CalculateGramSchmidtRotationMatrix(normal, noise_texture_sample.xyz * 2.0f - 1.0f);

	//Calculate the random hemisphere sample start index.
	uint random_hemisphere_sample_start_index = uint(noise_texture_sample.w * 64.0f);

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
float CastRayScene(vec3 ray_origin, vec3 ray_direction, float start_offset, out vec3 hit_radiance)
{
	//Calculate the screen space origin.
	vec3 screen_space_origin;

	{
		vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(ray_origin, 1.0f);
		float screen_coordinate_inverse_denominator = 1.0f / view_space_position.w;
		screen_space_origin = view_space_position.xyz * screen_coordinate_inverse_denominator;
		screen_space_origin.xy = screen_space_origin.xy * 0.5f + 0.5f;
		screen_space_origin.z = LinearizeDepth(screen_space_origin.z);
	}

	//Calculate the screen space direction.
	vec3 screen_space_direction;

	{
		vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(ray_origin + ray_direction, 1.0f);
		float screen_coordinate_inverse_denominator = 1.0f / view_space_position.w;
		screen_space_direction = view_space_position.xyz * screen_coordinate_inverse_denominator;
		screen_space_direction.xy = screen_space_direction.xy * 0.5f + 0.5f;
		screen_space_direction.z = LinearizeDepth(screen_space_direction.z);

		screen_space_direction = normalize(screen_space_direction - screen_space_origin);
	}

	//Perform the raycast.
	float current_step = SCREEN_SPACE_INDIRECT_LIGHTING_RAY_STEP * start_offset;

	for (uint i = 0; i < SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES; ++i)
	{
		//Calculate the sample position.
		vec3 screen_space_sample_position = screen_space_origin + screen_space_direction * current_step;

		//Terminate the ray if outside the screen.
		if (!ValidCoordinate(screen_space_sample_position.xy))
		{
			return 0.0f;
		}

		//Sample the depth at the current sample position.
		vec4 sample_scene_features_2 = texture(scene_features_2_texture, screen_space_sample_position.xy);
		float sample_depth = LinearizeDepth(sample_scene_features_2.w);

		//If the sample position's depth is greater then the sample depth, there is a hit!
		if (screen_space_sample_position.z < sample_depth)
		{
			//Test the (world space) surface normal against the (world space) ray direction.
			if (dot(ray_direction, sample_scene_features_2.xyz) <= 0.0f)
			{
				//Test the (world space) direction towards the hit position against the (world space) ray direction.
				vec3 hit_position = CalculateWorldPosition(sample_scene_features_2.xy, sample_scene_features_2.w);
				float distance_to_hit_position = length(hit_position - ray_origin);;
				vec3 direction_to_hit_position = vec3(hit_position - ray_origin) / distance_to_hit_position;

				if (dot(ray_direction, direction_to_hit_position) >= 0.0f)
				{
					//Sample the scene at the sample screen coordinate.
					hit_radiance = texture(scene_texture, screen_space_sample_position.xy).rgb;

					//Since the hit radiance only contains emissive and direct lighting, add some simple indirect and volumetric lighting to the point.
					{
						//Sample the sample's material properties.
						vec4 sample_scene_features_1 = texture(scene_features_1_texture, screen_space_sample_position.xy);
						vec4 sample_scene_features_3 = texture(scene_features_3_texture, screen_space_sample_position.xy);

						//Add indirect lighting.
						{
							//Sample the sky.
							vec3 sky_diffuse_sample = SampleSkyDiffuse(sample_scene_features_2.xyz);
							vec3 sky_specular_sample = SampleSkySpecular(ray_direction, sample_scene_features_2.xyz, sample_scene_features_3[0], sample_scene_features_3[1]);

							//Calculate the indirect lighting.
							vec3 indirect_lighting = CalculatePrecomputedLighting(	-ray_direction,
																					sample_scene_features_1.rgb,
																					sample_scene_features_2.xyz,
																					sample_scene_features_3[0],
																					sample_scene_features_3[1],
																					sample_scene_features_3[2],
																					1.0f,
																					sky_diffuse_sample,
																					sky_specular_sample,
																					vec2(1.0f, 0.0f));

							//Blend in volumetric lighting into the indirect lighting to mesh better with the scene.
							vec3 volumetric_ambient_lighting = CalculateVolumetricAmbientLighting();
							float volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(VIEW_DISTANCE, VOLUMETRIC_LIGHTING_DISTANCE, vec3(hit_position + sample_scene_features_2.xyz * VIEW_DISTANCE).y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, hit_position.y);

							indirect_lighting = mix(indirect_lighting, volumetric_ambient_lighting, volumetric_lighting_opacity);

							hit_radiance += indirect_lighting;

							//Blend in volumetric lighting into the hit radiance to mesh better with the scene.
							volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(distance_to_hit_position, VOLUMETRIC_LIGHTING_DISTANCE, hit_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, ray_origin.y);
						
							hit_radiance = mix(hit_radiance, volumetric_ambient_lighting, volumetric_lighting_opacity);
						}
					}

					//Return that there was a hit.
					return 1.0f;
				}
			}
		}

		current_step += SCREEN_SPACE_INDIRECT_LIGHTING_RAY_STEP;
	}

	//There was no hit.
	return 0.0f;
}

void CatalystShaderMain()
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

	//Fire all rays.
	vec3 indirect_lighting = vec3(0.0f);
	float total_weight = 0.0f;

	for (uint i = 0; i < SCREEN_SPACE_INDIRECT_LIGHTING_SAMPLES; ++i)
	{
		//Calculate the ray direction and start offset.
		vec3 ray_direction;
		float start_offset;

		CalculateIndirectLightingRayDirectionAndStartOffset(i, view_direction, normal, roughness, metallic, ray_direction, start_offset);

		//Calculate the indirect lighting.
		vec3 sample_indirect_lighting;

		float hit = CastRayScene(world_position + ray_direction , ray_direction, start_offset, sample_indirect_lighting);

		//Calculate the sample weight.
		float sample_weight = ProbabilityDensityFunction(normal, ray_direction, roughness, metallic) * hit;

		//Accumulate.
		indirect_lighting += hit > 0.0f ? sample_indirect_lighting * sample_weight : vec3(0.0f);
		total_weight += sample_weight;
	}

	//Normalize the indirect lighting.
	indirect_lighting = total_weight != 0.0f ? indirect_lighting / total_weight : vec3(0.0f);

    //Write the fragment
    fragment = vec4(indirect_lighting, min(total_weight, 1.0f));
}