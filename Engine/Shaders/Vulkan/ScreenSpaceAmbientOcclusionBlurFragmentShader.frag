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

    return mix(sampleOcclusion, currentOcclusion, SmoothStep(min(abs(currentDepth - sampleDepth) * depthMultiplier, 1.0f)));
}

/*
*   The blur function.
*/
float Blur()
{
    #define SAMPLE_CONTRIBUTION (0.0909090909090909f)

    float currentOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;
    float currentDepth = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, texture(normalDepthTexture, fragmentTextureCoordinate).w).z;
    float depthMultiplier = max(4.0025f - (abs(currentDepth) * 0.0275f), 0.0f); //0.0025f step.

    vec2 offset1 = vec2(1.0f) * direction * inverseResolution;
    vec2 offset2 = vec2(2.0f) * direction * inverseResolution;
    vec2 offset3 = vec2(3.0f) * direction * inverseResolution;
    vec2 offset4 = vec2(4.0f) * direction * inverseResolution;
    vec2 offset5 = vec2(5.0f) * direction * inverseResolution;

    return  currentOcclusion * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset1, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset1, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset2, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset2, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset3, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset3, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset4, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset4, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset5, depthMultiplier) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset5, depthMultiplier) * SAMPLE_CONTRIBUTION;
}

void main()
{
    //Write the fragment.
    fragment = vec4(Blur());
}