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
    layout (offset = 0) vec2 direction;
    layout (offset = 8) vec2 inverseResolution;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D screenSpaceAmbientOcclusionTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   The sample function.
*/
float Sample(float currentOcclusion, float currentDepth, vec2 coordinate, float depthMultiplier)
{
    float sampleOcclusion = texture(screenSpaceAmbientOcclusionTexture, coordinate).x;
    float sampleDepth = CalculateFragmentViewSpacePosition(coordinate, texture(normalDepthTexture, coordinate).w).z;

    return mix(sampleOcclusion, currentOcclusion, SmoothStep(min(abs(currentDepth - sampleDepth), 1.0f)));
}

/*
*   The blur function.
*/
float Blur()
{
    #define SAMPLE_CONTRIBUTION (1.0f / 21.0f)

    float currentOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;
    float currentDepth = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, texture(normalDepthTexture, fragmentTextureCoordinate).w).z;
    float depthMultiplier = max(4.0f - (abs(currentDepth) * 0.075f), 0.0f); //0.0025f step.
    //float depthMultiplier = 0.0f;

    vec2 offset1 = vec2(0.5f) * direction * inverseResolution;
    vec2 offset2 = vec2(2.5f) * direction * inverseResolution;
    vec2 offset3 = vec2(4.5f) * direction * inverseResolution;
    vec2 offset4 = vec2(6.5f) * direction * inverseResolution;
    vec2 offset5 = vec2(8.5f) * direction * inverseResolution;
    vec2 offset6 = vec2(10.5f) * direction * inverseResolution;
    vec2 offset7 = vec2(12.5f) * direction * inverseResolution;
    vec2 offset8 = vec2(14.5f) * direction * inverseResolution;
    vec2 offset9 = vec2(16.5f) * direction * inverseResolution;
    vec2 offset10 = vec2(18.5f) * direction * inverseResolution;

    return  (currentOcclusion
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset1, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset1, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset2, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset2, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset3, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset3, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset4, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset4, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset5, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset5, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset6, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset6, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset7, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset7, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset8, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset8, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset9, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset9, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset10, depthMultiplier)
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset10, depthMultiplier)) * SAMPLE_CONTRIBUTION;
}

void main()
{
    //Write the fragment.
    fragment = vec4(Blur());
}