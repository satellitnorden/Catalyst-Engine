//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Applies FXAA.
*/
vec3 ApplyFastApproximateAntiAliasing(vec3 fragment)
{
    //Calculate the anti aliased fragment.
    vec3 antiAliasedFragment = fragment;
    float antiAliasedFragmentWeight = 1.0f;

    //Calculate the average of this fragment.
    float average = CalculateAverage(fragment);

    //Sample the neighboring pixels.
    vec3 indirectSamples[4] = vec3[]
    (  
        texture(sceneTexture, fragmentTextureCoordinate + vec2(-1.0f, -1.0f) * inverseScaledResolution).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(-1.0f, 1.0f) * inverseScaledResolution).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(1.0f, 1.0f) * inverseScaledResolution).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(1.0f, -1.0f) * inverseScaledResolution).rgb
    );

    for (int i = 0; i < 4; ++i)
    {
        //Calculate the average for the sample.
        float sampleAverage = CalculateAverage(indirectSamples[i]);

        //Calculate the weight.
        float weight = abs(average - sampleAverage) * 0.125f;

        //Blend in the sample based on the weight.
        antiAliasedFragment += indirectSamples[i] * weight;
        antiAliasedFragmentWeight += weight;
    }

    vec3 straightSamples[4] = vec3[]
    (  
        texture(sceneTexture, fragmentTextureCoordinate + vec2(-1.0f, 0.0f) * inverseScaledResolution).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(1.0f, 0.0f) * inverseScaledResolution).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, -1.0f) * inverseScaledResolution).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, 1.0f) * inverseScaledResolution).rgb
    );

    for (int i = 0; i < 4; ++i)
    {
        //Calculate the average for the sample.
        float sampleAverage = CalculateAverage(straightSamples[i]);

        //Calculate the weight.
        float weight = abs(average - sampleAverage) * 0.25f;

        //Blend in the sample based on the weight.
        antiAliasedFragment += straightSamples[i] * weight;
        antiAliasedFragmentWeight += weight;
    }

    //Normalize the anti aliased fragment.
    antiAliasedFragment /= antiAliasedFragmentWeight;

    //Return the anti aliased fragment.
    return antiAliasedFragment;
}

void main()
{
    //Sample the scene color.
    vec3 sceneColor = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply fast approximate anti aliasing.
    if (antiAliasingMode == ANTI_ALIASING_MODE_FAST_APPROXIMATE
        || antiAliasingMode == ANTI_ALIASING_MODE_FAST_APPROXIMATE_AND_TEMPORAL)
    {
        sceneColor = ApplyFastApproximateAntiAliasing(sceneColor);
    }

    //Write the fragment.
    fragment = vec4(sceneColor, 1.0f);
}