//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;

//Out parameters.
layout (location = 0) out vec4 scene_features_2_half;

void main()
{
	//Calculate the texel offset.
	vec2 texel_offset = vec2(inverseScaledResolution.x * 0.5f, inverseScaledResolution.y * 0.5f);

	//Compare the depths in the scene features 2 texture. Choose the texel for the downsample with the depth closest to the perceiver.
	vec4 scene_features_2_sample_1 = texture(scene_features_2_texture, fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_2 = texture(scene_features_2_texture, fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y));
	vec4 scene_features_2_sample_3 = texture(scene_features_2_texture, fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_4 = texture(scene_features_2_texture, fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y));

    //Write the fragments.
    if (scene_features_2_sample_1.w > scene_features_2_sample_2.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_3.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_4.w)
    {
    	scene_features_2_half = scene_features_2_sample_1;
    }

    else if (	scene_features_2_sample_2.w > scene_features_2_sample_3.w
    			&& scene_features_2_sample_2.w > scene_features_2_sample_4.w)
    {
    	scene_features_2_half = scene_features_2_sample_2;
    }

    else if (scene_features_2_sample_3.w > scene_features_2_sample_4.w)
    {
    	scene_features_2_half = scene_features_2_sample_3;
    }

    else
    {
    	scene_features_2_half = scene_features_2_sample_4;
    }
}