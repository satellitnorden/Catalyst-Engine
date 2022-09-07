//Includes.
#include "CatalystGeometryMath.glsl"
#include "CatalystIndirectLightingCore.glsl"

//Constants.
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
layout (set = 1, binding = 3) uniform sampler2D depth_mip_chain[8];

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Calculates the indirect lighting ray direction and start offset.
*/
void CalculateIndirectLightingRayDirectionAndStartOffset(uint index, vec3 view_direction, vec3 normal, float roughness, out vec3 ray_direction, out float start_offset)
{
	//Sample the noise texture.
	vec4 noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), index);

	//Generate the ray direction.
	ray_direction = CatalystShaderGenerateSpecularLobeDirectionVector(vec2(noise_texture_sample[0], noise_texture_sample[1]), reflect(view_direction, normal), roughness);

	//Write the start offset.
	start_offset = noise_texture_sample[2];
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
float CastRayScene(vec4 scene_features_1, vec4 scene_features_2, vec4 scene_features_3, vec3 view_direction, vec3 ray_origin, vec3 ray_direction, float start_offset, out ScreenSpaceIndirectLightingData output_data)
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
					//Fill out the output_data.
					output_data._HitPosition = hit_position;

					//Return the hit percentage.
					float hit_percentage_x = abs(0.5f - screen_space_sample_position.x) * 2.0f;
					hit_percentage_x *= hit_percentage_x;
					hit_percentage_x = 1.0f - hit_percentage_x;

					float hit_percentage_y = abs(0.5f - screen_space_sample_position.y) * 2.0f;
					hit_percentage_y *= hit_percentage_y;
					hit_percentage_y = 1.0f - hit_percentage_y;

					return hit_percentage_x * hit_percentage_y;
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

	//Calculate the ray direction and start offset.
	vec3 ray_direction;
	float start_offset;

	CalculateIndirectLightingRayDirectionAndStartOffset(0, view_direction, scene_features_2.xyz, scene_features_3[0], ray_direction, start_offset);

	//Calculate the screen space indirect lighting data.
	ScreenSpaceIndirectLightingData screen_space_indirect_lighting_data;

	screen_space_indirect_lighting_data._HitPosition = vec3(0.0f);
	screen_space_indirect_lighting_data._HitPercentage = 0.0f;

	//Is the ray pointing in the right direction?
	if (dot(ray_direction, scene_features_2.xyz) > 0.0f)
	{
		screen_space_indirect_lighting_data._HitPercentage = CastRayScene(scene_features_1, scene_features_2, scene_features_3, view_direction, world_position, ray_direction, start_offset, screen_space_indirect_lighting_data);
	}

	//Pack the screen space indirect lighting data.
	vec4 packed_screen_space_indirect_lighting_data = PackScreenSpaceIndirectLightingData(screen_space_indirect_lighting_data);

    //Write the fragment
    fragment = packed_screen_space_indirect_lighting_data;
}