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
layout (set = 1, binding = 0) uniform sampler2D cloud_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//calculate the result.
	vec4 result = vec4(0.0f);

    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(-inverseScaledResolution.x, -inverseScaledResolution.y));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(-inverseScaledResolution.x, 0.0f));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(-inverseScaledResolution.x, inverseScaledResolution.y));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(0.0f, -inverseScaledResolution.y));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(0.0f, 0.0f));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(0.0f, inverseScaledResolution.y));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(inverseScaledResolution.x, -inverseScaledResolution.y));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(inverseScaledResolution.x, 0.0f));
    result += Upsample(cloud_texture, fragment_texture_coordinate + vec2(inverseScaledResolution.x, inverseScaledResolution.y));

    //Write the fragment.
    fragment = result / 9.0f;
}