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
float Sample(float currentOcclusion, float currentDepth, vec2 coordinate)
{
    float sampleOcclusion = texture(screenSpaceAmbientOcclusionTexture, coordinate).x;
    float sampleDepth = CalculateFragmentViewSpacePosition(coordinate, texture(normalDepthTexture, coordinate).w).z;

    return mix(sampleOcclusion, currentOcclusion, min(abs(currentDepth - sampleDepth), 1.0f));
}

/*
*   The blur function.
*/
float Blur()
{
    #define SAMPLE_CONTRIBUTION (0.0769230769230769f)

    float currentOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;
    float currentDepth = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, texture(normalDepthTexture, fragmentTextureCoordinate).w).z;

    vec2 offset1 = vec2(1.0f) * direction * inverseResolution;
    vec2 offset2 = vec2(2.0f) * direction * inverseResolution;
    vec2 offset3 = vec2(3.0f) * direction * inverseResolution;
    vec2 offset4 = vec2(4.0f) * direction * inverseResolution;
    vec2 offset5 = vec2(5.0f) * direction * inverseResolution;
    vec2 offset6 = vec2(6.0f) * direction * inverseResolution;

    return  currentOcclusion * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset1) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset1) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset2) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset2) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset3) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset3) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset4) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset4) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset5) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset5) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset6) * SAMPLE_CONTRIBUTION
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset6) * SAMPLE_CONTRIBUTION;
}

void main()
{
    //Write the fragment.
    fragment = vec4(Blur());
}