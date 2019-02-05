//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 inverseResolution;
    layout (offset = 8) float inverseDepthOfFieldDistanceSquared;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Forward declarations.
vec4 CalculateBlur();
vec4 SampleDepthOfField(vec2 textureCoordinate);

/*
*   Samples the depth of field at the given texture coordinate.
*/
vec4 SampleDepthOfField(vec2 textureCoordinate)
{
    //Sample the scene texture.
    vec4 sceneTexture = texture(sceneTexture, textureCoordinate);

    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, textureCoordinate);

    //Calculate the fragment world position.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, normalDepthTextureSampler.w);

    //Calculate the distance to the fragment world position squared.
    float distanceSquared = LengthSquared3(cameraWorldPosition - fragmentWorldPosition);

    //Calculate the depth of field weight.
    float depthOfFieldWeight = min(distanceSquared * inverseDepthOfFieldDistanceSquared, 1.0f) * float(normalDepthTextureSampler.w != 0.0f);

    //Write the fragment.
    return vec4(sceneTexture.rgb, depthOfFieldWeight);
}

/*
*   Calculates blur.
*/
vec4 CalculateBlur()
{
    vec2 offset1 = vec2(1.0f) * inverseResolution;
    vec2 offset2 = vec2(2.0f) * inverseResolution;
    vec2 offset3 = vec2(3.0f) * inverseResolution;

    return  SampleDepthOfField(fragmentTextureCoordinate) * 0.1428571428571429f
            + SampleDepthOfField(fragmentTextureCoordinate - offset1) * 0.1428571428571429f
            + SampleDepthOfField(fragmentTextureCoordinate + offset1) * 0.1428571428571429f
            + SampleDepthOfField(fragmentTextureCoordinate - offset2) * 0.1428571428571429f
            + SampleDepthOfField(fragmentTextureCoordinate + offset2) * 0.1428571428571429f
            + SampleDepthOfField(fragmentTextureCoordinate - offset3) * 0.1428571428571429f
            + SampleDepthOfField(fragmentTextureCoordinate + offset3) * 0.1428571428571429f;
}

void main()
{
    //Write the fragment.
    fragment = CalculateBlur();
}