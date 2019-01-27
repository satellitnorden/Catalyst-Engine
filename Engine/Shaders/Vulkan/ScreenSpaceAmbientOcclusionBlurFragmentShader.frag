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
    layout (offset = 16) float maximumDepthDifference;
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
    float sampleDepth = texture(normalDepthTexture, coordinate).w;

    return abs(currentDepth - sampleDepth) > maximumDepthDifference ? currentOcclusion : sampleOcclusion;
}

/*
*   The blur function.
*/
float Blur()
{
    float currentOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;
    float currentDepth = texture(normalDepthTexture, fragmentTextureCoordinate).w;

    vec2 offset1 = vec2(1.5f) * direction * inverseResolution;
    vec2 offset2 = vec2(3.0f) * direction * inverseResolution;
    vec2 offset3 = vec2(4.5f) * direction * inverseResolution;
    vec2 offset4 = vec2(6.0f) * direction * inverseResolution;
    vec2 offset5 = vec2(7.5f) * direction * inverseResolution;
    vec2 offset6 = vec2(9.0f) * direction * inverseResolution;

    return  currentOcclusion * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset1) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset1) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset2) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset2) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset3) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset3) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset4) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset4) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset5) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset5) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset6) * 0.0588235294117647f
            + Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset6) * 0.0588235294117647f;
}

void main()
{
    //Write the fragment.
    fragment = vec4(Blur());
}