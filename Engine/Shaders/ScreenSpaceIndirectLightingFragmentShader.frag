//Includes.
#include "CatalystGeometryMath.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Constants.
#define SCREEN_SPACE_INDIRECT_LIGHTING_SAMPLES (1)
#define SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES (16)
#define SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES_RECIPROCAL (1.0f / float(SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES))
#define SCREEN_SPACE_INDIRECT_LIGHTING_REFINE_STEPS (4)

#define CELL_STEP_OFFSET (0.05f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_features_3_texture;
layout (set = 1, binding = 3) uniform sampler2D scene_texture;
layout (set = 1, binding = 4) uniform sampler2D depth_mip_chain[8];

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Calculates the indirect lighting ray direction and start offset.
*/
void CalculateIndirectLightingRayDirectionAndStartOffset(uint index, vec3 view_direction, vec3 normal, float roughness, float metallic, out vec3 ray_direction, out float start_offset)
{
	//Sample the noise texture.
	vec4 noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), index);

	//Calculate the random rotation matrix.
	mat3 random_rotation = CalculateGramSchmidtRotationMatrix(normal, noise_texture_sample.xyz * 2.0f - 1.0f);

	//Calculate the random hemisphere sample start index.
	uint random_hemisphere_sample_start_index = uint(noise_texture_sample.w * 64.0f);

	//Sample the random direction and length.
	vec3 random_hemisphere_direction;
	float random_length;

	SampleHammersleyHemisphereSample(index + random_hemisphere_sample_start_index + uint(gl_FragCoord.x) + uint(gl_FragCoord.y), random_hemisphere_direction, random_length);

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
*	Steps through a cell.
*/
vec3 StepThroughCell(vec3 screen_space_position, vec3 screen_space_direction, int mip_index)
{
#if 1
	ivec2 mip_size = ivec2(SCALED_RESOLUTION) / (1 << mip_index);
	vec2 mip_cell_index = screen_space_position.xy * vec2(mip_size);

	vec2 boundary_uv;

	boundary_uv.x = screen_space_direction.x > 0 ? ceil(mip_cell_index.x) / float(mip_size.x) :  floor(mip_cell_index.x) / float(mip_size.x);
	boundary_uv.y = screen_space_direction.y > 0 ? ceil(mip_cell_index.y) / float(mip_size.y) :  floor(mip_cell_index.y) / float(mip_size.y);

	vec2 t;

	t.x = (boundary_uv.x - screen_space_position.x) / screen_space_direction.x;
	t.y = (boundary_uv.y - screen_space_position.y) / screen_space_direction.y;

	// Pick the cell intersection that is closer, and march to that cell
	if (abs(t.x) < abs(t.y))
	{
		return screen_space_position + ((t.x + CELL_STEP_OFFSET / mip_size.x) * screen_space_direction);
	}

	else
	{
		return screen_space_position + ((t.y + CELL_STEP_OFFSET / mip_size.y) * screen_space_direction);
	}
#else
	return screen_space_position + screen_space_direction * 0.01f;
#endif
}

/*
*	Casts a ray against the scene. Returns if there was a hit.	
*/
float CastRayScene(vec4 scene_features_1, vec4 scene_features_2, vec4 scene_features_3, vec3 view_direction, vec3 ray_origin, vec3 ray_direction, float start_offset, out vec3 hit_radiance)
{
#if 0
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

	//Calculate the sample position.
	vec3 screen_space_sample_position = screen_space_origin + screen_space_direction * start_offset * SCREEN_SPACE_INDIRECT_LIGHTING_RAY_STEP;

	//Do HZB thingie.
	int mip_index = 0;
	uint iteration_count = 0;

	while (mip_index > -1 && iteration_count < 64)
	{
		//Is it a valid coordinate.
		if (!ValidCoordinate(screen_space_sample_position.xy))
		{
			return 0.0f;
		}

		//Step through a cell.
		screen_space_sample_position = StepThroughCell(screen_space_sample_position, screen_space_direction, mip_index);

		//Sample the depth at the current sample position.
		float sample_depth = texture(depth_mip_chain[mip_index], screen_space_sample_position.xy).x;

		//Linearize the depth.
		sample_depth = LinearizeDepth(sample_depth);

		//No intersection.
		if (sample_depth == 0.0f || screen_space_sample_position.z > sample_depth)
		{
			mip_index = min(mip_index + 1, 7);
		}

		//Intersection.
		else
		{
			float t = (sample_depth - screen_space_sample_position.z) / screen_space_direction.z;
			screen_space_sample_position -= screen_space_direction * t;

			--mip_index;
		}

		++iteration_count;
	}

	//Sample the scene radiance at the sample screen coordinate.
	vec3 scene_radiance = texture(scene_texture, screen_space_sample_position.xy).rgb;

	//The scene radiance at this point has only received direct lighting, so add some indirect lighting from the sky texture to simulate multiple bounces.
	{
		vec4 sample_scene_features_1 = texture(scene_features_1_texture, screen_space_sample_position.xy);
		vec4 sample_scene_features_2 = texture(scene_features_2_texture, screen_space_sample_position.xy);
		vec4 sample_scene_features_3 = texture(scene_features_3_texture, screen_space_sample_position.xy);

		vec3 sample_specular_direction = reflect(ray_direction, sample_scene_features_2.xyz);
		vec3 sample_diffuse_direction = sample_scene_features_2.xyz;

		float diffuse_weight = sample_scene_features_3[0] * (1.0f - sample_scene_features_3[1]);

		vec3 sample_ray_direction = normalize(mix(sample_specular_direction, sample_diffuse_direction, diffuse_weight));

		scene_radiance += CalculateLighting(-ray_direction,
											sample_scene_features_1.rgb,
											sample_scene_features_2.xyz,
											sample_scene_features_3[0],
											sample_scene_features_3[1],
											sample_scene_features_3[2],
											1.0f,
											-sample_ray_direction,
											SampleSky(sample_ray_direction, MAX_SKY_TEXTURE_MIPMAP_LEVEL * diffuse_weight));

		//Calculate the hit radiance.
		{
			hit_radiance = scene_radiance;
		}

		//Return that there was a hit.
		return 1.0f;
	}
#else
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

	//Retrieve the screen space end position.
	Ray ray;

	ray._Origin = screen_space_origin;
	ray._Direction = screen_space_direction;

	vec3 screen_space_end_position = RayViewportIntersection(ray);

	//Perform the raycast.
	for (uint i = 0; i < SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES; ++i)
	{
		//Calculate the sample position.
		vec3 screen_space_sample_position = mix(screen_space_origin, screen_space_end_position, (SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES_RECIPROCAL * start_offset) + (float(i) / float(SCREEN_SPACE_INDIRECT_LIGHTING_RAY_MAXIMUM_SAMPLES)));

		//Terminate the ray if outside the screen.
		if (!ValidCoordinate(screen_space_sample_position.xy))
		{
			return 0.0f;
		}

		//Sample the depth at the current sample position.
		float sample_depth = texture(depth_mip_chain[0], screen_space_sample_position.xy).x;

		//Discard hit's against the sky.
		if (sample_depth == 0.0f)
		{
			continue;
		}

		sample_depth = LinearizeDepth(sample_depth);

		//If the sample position's depth is greater then the sample depth, there is a hit!
		if (screen_space_sample_position.z < sample_depth)
		{
			//Sample scene features 2.
			vec4 sample_scene_features_2 = texture(scene_features_2_texture, screen_space_sample_position.xy);

			//Test the (world space) surface normal against the (world space) ray direction.
			if (dot(ray_direction, sample_scene_features_2.xyz) < 0.0f)
			{
				//Test the (world space) direction towards the hit position against the (world space) ray direction.
				vec3 hit_position = CalculateWorldPosition(screen_space_sample_position.xy, sample_scene_features_2.w);
				float distance_to_hit_position = length(hit_position - ray_origin);
				vec3 direction_to_hit_position = vec3(hit_position - ray_origin) / distance_to_hit_position;

				if (dot(ray_direction, direction_to_hit_position) > 0.0f)
				{
					//Sample the scene radiance at the sample screen coordinate.
					vec3 scene_radiance = texture(scene_texture, screen_space_sample_position.xy).rgb;

					//The scene radiance at this point has only received direct lighting, so add some indirect lighting from the sky texture to simulate multiple bounces.
					{
						vec4 sample_scene_features_1 = texture(scene_features_1_texture, screen_space_sample_position.xy);
						vec4 sample_scene_features_3 = texture(scene_features_3_texture, screen_space_sample_position.xy);

						vec3 sample_specular_direction = reflect(ray_direction, sample_scene_features_2.xyz);
						vec3 sample_diffuse_direction = sample_scene_features_2.xyz;

						float diffuse_weight = sample_scene_features_3[0] * (1.0f - sample_scene_features_3[1]);

						vec3 sample_ray_direction = normalize(mix(sample_specular_direction, sample_diffuse_direction, diffuse_weight));

						scene_radiance += CalculateLighting(-ray_direction,
															sample_scene_features_1.rgb,
															sample_scene_features_2.xyz,
															sample_scene_features_3[0],
															sample_scene_features_3[1],
															sample_scene_features_3[2],
															1.0f,
															-sample_ray_direction,
															SampleSky(sample_ray_direction, MAX_SKY_TEXTURE_MIPMAP_LEVEL * diffuse_weight));
					}

					//Calculate the hit radiance.
					{
						hit_radiance = scene_radiance;
					}

					//Return that there was a hit.
					return 1.0f;
				}
			}
		}
	}

	//There was no hit.
	return 0.0f;
#endif
}

void CatalystShaderMain()
{
	//Sample the scene features.
	vec4 scene_features_1 = texture(scene_features_1_texture, fragment_texture_coordinate);
	vec4 scene_features_2 = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec4 scene_features_3 = texture(scene_features_3_texture, fragment_texture_coordinate);

	//Calculate the world position at this fragment.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w);

	//Calculate the view direction.
	vec3 view_direction = normalize(world_position - CAMERA_WORLD_POSITION);

	//Fire all rays.
	vec3 indirect_lighting = vec3(0.0f);
	float total_weight = 0.0f;

	for (uint i = 0; i < SCREEN_SPACE_INDIRECT_LIGHTING_SAMPLES; ++i)
	{
		//Calculate the ray direction and start offset.
		vec3 ray_direction;
		float start_offset;

		CalculateIndirectLightingRayDirectionAndStartOffset(i, view_direction, scene_features_2.xyz, scene_features_3[0], scene_features_3[1], ray_direction, start_offset);

		//Calculate the indirect lighting.
		vec3 sample_indirect_lighting;

		float hit = CastRayScene(scene_features_1, scene_features_2, scene_features_3, view_direction, world_position, ray_direction, start_offset, sample_indirect_lighting);

		//Accumulate.
		indirect_lighting += hit > 0.0f ? sample_indirect_lighting * hit : vec3(0.0f);
		total_weight += hit;
	}

	//Normalize the indirect lighting.
	indirect_lighting = total_weight != 0.0f ? indirect_lighting / total_weight : vec3(0.0f);

    //Write the fragment
    fragment = vec4(indirect_lighting, min(total_weight, 1.0f));
}