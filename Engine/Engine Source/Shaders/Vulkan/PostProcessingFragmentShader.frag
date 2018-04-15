//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    layout (offset = 0) float cameraFieldOfViewCosine; //Offset; 0 - Size; 16
    layout (offset = 16) mat4 inverseCameraMatrix; //Offset; 16 - Size; 64
    layout (offset = 80) mat4 inverseProjectionMatrix; //Offset; 80 - Size; 64
    layout (offset = 144) mat4 originViewMatrix; //Offset; 144 - Size; 64
    layout (offset = 208) mat4 viewMatrix; //Offset; 208 - Size; 64
    layout (offset = 272) vec3 cameraForwardVector; //Offset; 272 - Size; 16
    layout (offset = 288) vec3 cameraWorldPosition; //Offset; 288 - Size; 16

    //Directional light data.
    layout (offset = 304) float directionalLightIntensity; //Offset; 304 - Size; 16
    layout (offset = 320) vec3 directionalLightDirection; //Offset; 320 - Size; 16
    layout (offset = 336) vec3 directionalLightColor; //Offset; 336 - Size; 16
    layout (offset = 352) vec3 directionalLightScreenSpacePosition; //Offset; 352 - Size; 16

    //General data.
    layout (offset = 368) float deltaTime; //Offset; 368 - Size; 4
    layout (offset = 372) float randomSeed; //Offset; 372 - Size; 4
    layout (offset = 376) float totalGameTime; //Offset; 376 - Size; 4

    //Point light data.
    layout (offset = 380) int numberOfPointLights; //Offset; 380 - Size; 4
    layout (offset = 384) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 384 - Size; 128
    layout (offset = 512) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 512 - Size; 128
    layout (offset = 640) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 640 - Size; 128
    layout (offset = 768) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 768 - Size; 128

    //Spot light data.
    layout (offset = 896) int numberOfSpotLights; //Offset; 896 - Size; 16
    layout (offset = 912) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 912 - Size; 128
    layout (offset = 1040) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1040 - Size; 128
    layout (offset = 1168) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1168 - Size; 128
    layout (offset = 1296) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1296 - Size; 128
    layout (offset = 1424) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1424 - Size; 128
    layout (offset = 1552) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1552 - Size; 128
    layout (offset = 1680) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1680 - Size; 128

    //Physics data.
    layout (offset = 1808) float windStrength; //Offset; 1808 - Size; 16
    layout (offset = 1824) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1840
};

//Post processing data.
layout (std140, set = 1, binding = 1) uniform PostProcessingUniformData
{
    float blurAmount;
    float chromaticAberrationAmount;
    float saturation;
    float sharpenAmount;
};

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D sceneTexture;

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

/*
*   Applies HDR .
*/
vec3 ApplyHDR(vec3 fragment)
{
    return fragment / (fragment + vec3(1.0f));
}

/*
*   Applies gamma correction.
*/
vec3 ApplyGammaCorrection(vec3 fragment)
{
    return pow(fragment, vec3(1.0f / 2.2f));
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

    //Apply HDR.
    sceneTextureSampler = ApplyHDR(sceneTextureSampler);

    //Apply gamma correction.
    sceneTextureSampler = ApplyGammaCorrection(sceneTextureSampler);

    //Set the fragment color.
    fragmentColor = vec4(sceneTextureSampler, 1.0f);
}