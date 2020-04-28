//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_2_half;
layout (location = 1) out vec4 scene_features_3_half;
layout (location = 2) out vec4 scene_features_4_half;

void CatalystShaderMain()
{
	//Calculate the texel offset.
	vec2 texel_offset = vec2(INVERSE_SCALED_RESOLUTION.x * 0.5f, INVERSE_SCALED_RESOLUTION.y * 0.5f);

	//Compare the depths in the scene features 2 texture. Choose the texel for the downsample with the depth closest to the perceiver.
	vec4 scene_features_2_sample_1 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
	vec4 scene_features_2_sample_3 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_4 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));

    //Write the fragments.
    if (scene_features_2_sample_1.w > scene_features_2_sample_2.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_3.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_4.w)
    {
    	scene_features_2_half = scene_features_2_sample_1;
        scene_features_3_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
        scene_features_4_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
    }

    else if (	scene_features_2_sample_2.w > scene_features_2_sample_3.w
    			&& scene_features_2_sample_2.w > scene_features_2_sample_4.w)
    {
    	scene_features_2_half = scene_features_2_sample_2;
        scene_features_3_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
        scene_features_4_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
    }

    else if (scene_features_2_sample_3.w > scene_features_2_sample_4.w)
    {
    	scene_features_2_half = scene_features_2_sample_3;
        scene_features_3_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
        scene_features_4_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
    }

    else
    {
    	scene_features_2_half = scene_features_2_sample_4;
        scene_features_3_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));
        scene_features_4_half = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));
    }
}