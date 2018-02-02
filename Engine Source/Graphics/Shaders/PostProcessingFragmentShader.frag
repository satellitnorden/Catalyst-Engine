//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    mat4 inverseCameraMatrix;
    mat4 inverseProjectionMatrix;
    mat4 originViewMatrix;
    mat4 viewMatrix;
    vec3 cameraWorldPosition;
    float padding1;

    //Directional light data.
    float directionalLightIntensity;
    vec3 directionalLightDirection;
    vec3 directionalLightColor;
    float padding2;

    //Point light data.
    int numberOfPointLights;
    float pointLightAttenuationDistances[MaximumNumberOfPointLights];
    float pointLightIntensities[MaximumNumberOfPointLights];
    vec3 pointLightColors[MaximumNumberOfPointLights];
    vec3 pointLightWorldPositions[MaximumNumberOfPointLights];

    //Spot light data.
    int numberOfSpotLights;
    float spotLightAttenuationDistances[MaximumNumberOfSpotLights];
    float spotLightIntensities[MaximumNumberOfSpotLights];
    float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights];
    float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights];
    vec3 spotLightColors[MaximumNumberOfSpotLights];
    vec3 spotLightDirections[MaximumNumberOfSpotLights];
    vec3 spotLightWorldPositions[MaximumNumberOfSpotLights];
};

//Post processing data.
layout (std140, binding = 1) uniform PostProcessingUniformData
{
    float blurAmount;
    float chromaticAberrationAmount;
    float saturation;
    float sharpenAmount;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (binding = 2) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

//The sample offset.
float sampleOffset = 0.001f;

//The sample offsets.
vec2 sampleOffsets[9] = vec2[]
(
    vec2(-sampleOffset, -sampleOffset),     vec2(0.0f, -sampleOffset),  vec2(sampleOffset, -sampleOffset),
    vec2(-sampleOffset, 0.0f),              vec2(0.0f, 0.0f),           vec2(sampleOffset, 0.0f),
    vec2(-sampleOffset, sampleOffset),      vec2(0.0f, sampleOffset),   vec2(sampleOffset, sampleOffset)
);

//All the texture samples.
vec3 textureSamples[9] = vec3[]
(
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[0]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[1]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[2]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[3]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[4]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[5]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[6]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[7]).rgb,
    texture(sceneTexture, fragmentTextureCoordinate + sampleOffsets[8]).rgb
);

//The blur kernel.
float blurKernel[9] = float[]
(
    0.0625f,    0.125f,     0.0625f,
    0.125f,     0.25f,      0.125f,
    0.0625f,    0.125f,     0.0625f
);

//The sharpen kernel.
float sharpenKernel[9] = float[]
(
    -1.0f,  -1.0f,  -1.0f,
    -1.0f,  9.0f,   -1.0f,
    -1.0f,  -1.0f,  -1.0f
);

/*
*   Applies blur.
*/
vec3 ApplyBlur(vec3 sceneTextureSampler)
{
    //Calculate the blurred sample.
    vec3 blurredSample = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 9; ++i)
    {
        blurredSample += textureSamples[i] * blurKernel[i];
    }

    //Return the calculated color.
    return mix(sceneTextureSampler, blurredSample, blurAmount);
}

/*
*   Applies chromatic aberration.
*/
vec3 ApplyChromaticAberration(vec3 sceneTextureSampler)
{
    //Return the calculated color.
    return vec3(texture(sceneTexture, fragmentTextureCoordinate + vec2(chromaticAberrationAmount, chromaticAberrationAmount)).r, sceneTextureSampler.g, texture(sceneTexture, fragmentTextureCoordinate + vec2(-chromaticAberrationAmount, -chromaticAberrationAmount)).b);
}

/*
*   Applies saturation.
*/
vec3 ApplySaturation(vec3 sceneTextureSampler)
{
    //Calculate the average.
    float average = 0.2126 * sceneTextureSampler.r + 0.7152 * sceneTextureSampler.g + 0.0722 * sceneTextureSampler.b;

    //Calculate the grayscale color.
    vec3 grayscaleColor = vec3(average, average, average);

    //Return the calculated color.
    return mix (grayscaleColor, sceneTextureSampler, saturation);
}

/*
*   Applies sharpen.
*/
vec3 ApplySharpen(vec3 sceneTextureSampler)
{
    //Calculate the sharpened sample.
    vec3 sharpenedSample = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 9; ++i)
    {
        sharpenedSample += textureSamples[i] * sharpenKernel[i];
    }

    //Return the calculated color.
    return mix(sceneTextureSampler, sharpenedSample, sharpenAmount);
}

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureSampler = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply blur.
    sceneTextureSampler = ApplyBlur(sceneTextureSampler);

    //Apply chromatic aberration.
    sceneTextureSampler = ApplyChromaticAberration(sceneTextureSampler);

    //Apply saturation.
    sceneTextureSampler = ApplySaturation(sceneTextureSampler);

    //Apply sharpen.
    sceneTextureSampler = ApplySharpen(sceneTextureSampler);

    //Apply gamma correction.
    sceneTextureSampler = pow(sceneTextureSampler, vec3(1.0f / 2.2f));

    //Set the fragment color.
    fragmentColor = vec4(sceneTextureSampler, 1.0f);
}