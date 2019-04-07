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
vec3 ApplyFXAA(vec3 fragment)
{
	//Sample the 8 surrounding pixels.
	#define WIDTH_OFFSET (1.0f / 1920.0f)
	#define HEIGHT_OFFSET (1.0f / 1080.0f)

    //Calculate the average of this fragment.
    float average = CalculateAverage(fragment);

    //Sample the neighboring pixels.
    vec3 indirectSamples[4] = vec3[]
    (  
        texture(sceneTexture, fragmentTextureCoordinate + vec2(-WIDTH_OFFSET, -HEIGHT_OFFSET)).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(-WIDTH_OFFSET, HEIGHT_OFFSET)).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(WIDTH_OFFSET, HEIGHT_OFFSET)).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(WIDTH_OFFSET, -HEIGHT_OFFSET)).rgb
    );

    for (int i = 0; i < 4; ++i)
    {
        //Calculate the average for the sample.
        float sampleAverage = CalculateAverage(indirectSamples[i]);

        //Calculate the difference.
        float difference = pow(abs(average - sampleAverage), 1.25f);

        //Blend in the sample based on the difference.
        fragment = mix(fragment, mix(fragment, indirectSamples[i], 0.2f), difference);
    }

    vec3 straightSamples[4] = vec3[]
    (  
        texture(sceneTexture, fragmentTextureCoordinate + vec2(-WIDTH_OFFSET, 0.0f)).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(WIDTH_OFFSET, 0.0f)).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, -HEIGHT_OFFSET)).rgb,
        texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, HEIGHT_OFFSET)).rgb
    );

    for (int i = 0; i < 4; ++i)
    {
        //Calculate the average for the sample.
        float sampleAverage = CalculateAverage(straightSamples[i]);

        //Calculate the difference.
        float difference = pow(abs(average - sampleAverage), 1.25f);

        //Blend in the sample based on the difference.
        fragment = mix(fragment, mix(fragment, straightSamples[i], 0.8f), difference);
    }

    return fragment;
}

void main()
{
    //Sample the scene color.
    vec3 sceneColor = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply FXAA.
    //sceneColor = ApplyFXAA(sceneColor);

    //Write the fragment.
    fragment = vec4(sceneColor, 1.0f);
}