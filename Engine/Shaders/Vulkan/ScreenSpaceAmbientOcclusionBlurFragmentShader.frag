//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER (1.000225f) //0.000025f step. Increase to increase bias to samples of the same depth when blurring.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER_DECREASE (0.001225f) //0.000025f step. Increase to descrase bias to samples of the same depth when blurring at a distance.

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
    const int steps = 13;
    const float offsets[13] = float[13](
        0.0000000,
        1.4992315,
        3.4982069,
        5.4971822,
        7.4961576,
        9.4951330,
        11.4941085,
        13.4930840,
        15.4920596,
        17.4910352,
        19.4900109,
        21.4889867,
        23.4879626
    );
    const float weights[13] = float[13](
        0.0246499,
        0.0491738,
        0.0486728,
        0.0477837,
        0.0465282,
        0.0449359,
        0.0430440,
        0.0408954,
        0.0385369,
        0.0360182,
        0.0333894,
        0.0306999,
        0.0279967
    );

    float currentOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;
    float currentDepth = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, texture(normalDepthTexture, fragmentTextureCoordinate).w).z;
    float depthMultiplier = SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER - (abs(currentDepth) * SCREEN_SPACE_AMBIENT_OCCLUSION_BLUR_BASE_MULTIPLIER_DECREASE);

    float final = currentOcclusion * weights[0];

    for (int i = 1; i < steps; ++i)
    {
        final += Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate - offsets[i] * inverseResolution * direction, depthMultiplier) * weights[i];
        final += Sample(currentOcclusion, currentDepth, fragmentTextureCoordinate + offsets[i] * inverseResolution * direction, depthMultiplier) * weights[i];
    }

    return final;
}

void main()
{
    //Write the fragment.
    fragment = vec4(Blur());
}