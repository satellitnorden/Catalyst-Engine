//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D depth_of_field_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the depth.
	float depth = LinearizeDepth(texture(scene_features_2_texture, fragment_texture_coordinate).w);

	//Sample the depth of field.
	vec3 depth_of_field = texture(depth_of_field_texture, fragment_texture_coordinate).rgb;

	//Calculate the weight.
	float weight = 1.0f - depth;

    //Write the fragment.
    fragment = vec4(depth_of_field, weight);
}