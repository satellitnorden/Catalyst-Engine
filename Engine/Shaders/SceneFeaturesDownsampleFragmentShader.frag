//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_1_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_2_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D SCENE_FEATURES_3_TEXTURE;
layout (set = 1, binding = 3) uniform sampler2D SCENE_FEATURES_4_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 scene_features_1_half;
layout (location = 1) out vec4 scene_features_2_half;
layout (location = 2) out vec4 scene_features_3_half;
layout (location = 3) out vec4 scene_features_4_half;

void CatalystShaderMain()
{
	//Calculate the texel offset.
	vec2 texel_offset = vec2(INVERSE_SCALED_RESOLUTION.x * 0.5f, INVERSE_SCALED_RESOLUTION.y * 0.5f);

	//Compare the depths in the scene features 2 texture. Choose the texel for the downsample with the depth closest to the perceiver.
	vec4 scene_features_2_sample_1 = texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_2 = texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
	vec4 scene_features_2_sample_3 = texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_4 = texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));

    //Write the fragments.
    if (scene_features_2_sample_1.w > scene_features_2_sample_2.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_3.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_4.w)
    {
        scene_features_1_half = texture(SCENE_FEATURES_1_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
    	scene_features_2_half = scene_features_2_sample_1;
        scene_features_3_half = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
        scene_features_4_half = texture(SCENE_FEATURES_4_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
    }

    else if (	scene_features_2_sample_2.w > scene_features_2_sample_3.w
    			&& scene_features_2_sample_2.w > scene_features_2_sample_4.w)
    {
        scene_features_1_half = texture(SCENE_FEATURES_1_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
    	scene_features_2_half = scene_features_2_sample_2;
        scene_features_3_half = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
        scene_features_4_half = texture(SCENE_FEATURES_4_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
    }

    else if (scene_features_2_sample_3.w > scene_features_2_sample_4.w)
    {
        scene_features_1_half = texture(SCENE_FEATURES_1_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
    	scene_features_2_half = scene_features_2_sample_3;
        scene_features_3_half = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
        scene_features_4_half = texture(SCENE_FEATURES_4_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
    }

    else
    {
        scene_features_1_half = texture(SCENE_FEATURES_1_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));
    	scene_features_2_half = scene_features_2_sample_4;
        scene_features_3_half = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));
        scene_features_4_half = texture(SCENE_FEATURES_4_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));
    }
}