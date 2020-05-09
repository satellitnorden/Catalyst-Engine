//Constants.
#define SHADOW_MAP_SAMPLES (4)
#define SHADOW_MAP_OFFSET (0.00125f) //0.00025f step.
#define SHADOW_MAP_BIAS (0.00012f) //0.0000025f step.

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 WORLD_TO_LIGHT_MATRIX;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Sample the scene features 2.
	vec4 scene_features_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Calculate the world position.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w);

	//Calculate the shadow map coordinate and depth.
	vec2 shadow_map_coordinate;
	float shadow_map_depth;

	{
		vec4 light_space_position = WORLD_TO_LIGHT_MATRIX * vec4(world_position, 1.0f);
		shadow_map_coordinate = light_space_position.xy * 0.5f + 0.5f;
		shadow_map_depth = light_space_position.z;
	}

	vec2 shadow_map_offsets[SHADOW_MAP_SAMPLES];

	{
		vec4 blue_noise_texture_sample_1 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);
#if SHADOW_MAP_SAMPLES >= 4
		vec4 blue_noise_texture_sample_2 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 1);
#endif
#if SHADOW_MAP_SAMPLES >= 6
		vec4 blue_noise_texture_sample_3 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 2);
#endif
#if SHADOW_MAP_SAMPLES >= 8
		vec4 blue_noise_texture_sample_4 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 3);
#endif

		shadow_map_offsets[0] = blue_noise_texture_sample_1.xy * 2.0f - 1.0f;
		shadow_map_offsets[1] = blue_noise_texture_sample_1.zw * 2.0f - 1.0f;
#if SHADOW_MAP_SAMPLES >= 4
		shadow_map_offsets[2] = blue_noise_texture_sample_2.xy * 2.0f - 1.0f;
		shadow_map_offsets[3] = blue_noise_texture_sample_2.zw * 2.0f - 1.0f;
#endif
#if SHADOW_MAP_SAMPLES >= 6
		shadow_map_offsets[4] = blue_noise_texture_sample_3.xy * 2.0f - 1.0f;
		shadow_map_offsets[5] = blue_noise_texture_sample_3.zw * 2.0f - 1.0f;
#endif
#if SHADOW_MAP_SAMPLES >= 8
		shadow_map_offsets[6] = blue_noise_texture_sample_4.xy * 2.0f - 1.0f;
		shadow_map_offsets[7] = blue_noise_texture_sample_4.zw * 2.0f - 1.0f;
#endif
	}

	float shadow_factor = 0.0f;

	for (uint i = 0; i < SHADOW_MAP_SAMPLES; ++i)
	{

		vec2 offset_shadow_map_coordinate = shadow_map_coordinate + shadow_map_offsets[i] * SHADOW_MAP_OFFSET;

		float actual_shadow_map_depth = texture(sampler2D(RENDER_TARGETS[SHADOW_MAP_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), offset_shadow_map_coordinate).x;

		shadow_factor += ValidCoordinate(offset_shadow_map_coordinate) ? float(shadow_map_depth < actual_shadow_map_depth + SHADOW_MAP_BIAS) : 1.0f;
	}

	shadow_factor /= float(SHADOW_MAP_SAMPLES);

	//Write the fragment.
	fragment = vec4(shadow_factor, 0.0f, 0.0f, 1.0f);
}