//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER (1.000175f) //0.000025f step. Increase to increase bias to samples of the same depth when blurring.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER_DECREASE (0.00008f) //0.000025f step. Increase to descrase bias to samples of the same depth when blurring at a distance.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_SAMPLES (10)

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

    return mix(sampleOcclusion, currentOcclusion, SmoothStep(clamp(abs(currentDepth - sampleDepth) * depthMultiplier, 0.0f, 1.0f)));
}

/*
*   The blur function.
*/
float Blur()
{
    float currentOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;
    float currentDepth = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, texture(normalDepthTexture, fragmentTextureCoordinate).w).z;
    float depthMultiplier = SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER - (abs(currentDepth) * SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER_DECREASE);

    float final = currentOcclusion;

    for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_SAMPLES; ++i)
    {
        vec2 offset = (vec2(0.5f) + 2.0f * i) * inverseResolution * direction;

        final += Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offset, depthMultiplier);
        final += Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offset, depthMultiplier);
    }

    return final / (1 + (SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_SAMPLES * 2));
}

void main()
{
    //Write the fragment.
    fragment = vec4(Blur());
}