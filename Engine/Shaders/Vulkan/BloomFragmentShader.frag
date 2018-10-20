//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant
layout (push_constant) uniform BloomData
{
    float radius;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Calculates the average of a fragment.
*/
float CalculateAverage(vec4 fragment)
{
	return (fragment.r + fragment.b + fragment.b) * 0.33333333f;
}

/*
*   Calculates the bloom at a given texture coordinate.
*/
vec4 CalculateBloom(vec2 textureCoordinate)
{
    vec4 textureSampler = texture(sceneTexture, textureCoordinate);
    float average = CalculateAverage(textureSampler);

    return average > 1.0f ? vec4(textureSampler.rgb, average - 1.0f) : vec4(0.0f);
}


void main()
{
    vec4 finalFragment = vec4(0.0f);

    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(-radius, -radius)) * 0.0625f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(-radius, 0.0f)) * 0.125f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(-radius, radius)) * 0.0625f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(0.0f, -radius)) * 0.125f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(0.0f, 0.0f)) * 0.25f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(0.0f, radius)) * 0.125f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(radius, -radius)) * 0.0625f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(radius, 0.0f)) * 0.125f;
    finalFragment += CalculateBloom(fragmentTextureCoordinate + vec2(radius, radius)) * 0.0625f;

    finalFragment *= 0.111111f;

    //Write the fragment.
    fragment = finalFragment;
}