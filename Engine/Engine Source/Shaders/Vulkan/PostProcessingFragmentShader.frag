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
    layout (offset = 320) mat4 directionalLightViewMatrix; //Offset; 320 - Size; 64
    layout (offset = 384) vec3 directionalLightDirection; //Offset; 384 - Size; 16
    layout (offset = 400) vec3 directionalLightColor; //Offset; 400 - Size; 16
    layout (offset = 416) vec3 directionalLightScreenSpacePosition; //Offset; 416 - Size; 16

    //Environment data.
    layout (offset = 432) float environmentBlend; //Offset; 432 - Size; 4

    //General data.
    layout (offset = 436) float deltaTime; //Offset; 436 - Size; 4
    layout (offset = 440) float totalGameTime; //Offset; 440 - Size; 4

    //Point light data.
    layout (offset = 444) int numberOfPointLights; //Offset; 444 - Size; 4
    layout (offset = 448) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 448 - Size; 128
    layout (offset = 576) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 576 - Size; 128
    layout (offset = 704) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 704 - Size; 128
    layout (offset = 832) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 832 - Size; 128

    //Spot light data.
    layout (offset = 960) int numberOfSpotLights; //Offset; 960 - Size; 16
    layout (offset = 976) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 976 - Size; 128
    layout (offset = 1104) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1104 - Size; 128
    layout (offset = 1232) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1232 - Size; 128
    layout (offset = 1360) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1360 - Size; 128
    layout (offset = 1488) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1488 - Size; 128
    layout (offset = 1616) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1616 - Size; 128
    layout (offset = 1744) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1744 - Size; 128

    //Physics data.
    layout (offset = 1872) float windStrength; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//Post processing data.
layout (std140, set = 1, binding = 1) uniform PostProcessingUniformData
{
    float bloomStrength;
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
layout (set = 1, binding = 3) uniform sampler2D bloomTexture;

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
*   Applies bloom.
*/
vec3 ApplyBloom(vec3 fragment)
{
    if (bloomStrength > 0.0f)
    {
        //Sample the bloom texture.
        vec4 bloomTextureSampler = texture(bloomTexture, fragmentTextureCoordinate);

        //Apply the bloom.
        return fragment + (bloomTextureSampler.rgb * bloomTextureSampler.a * bloomStrength);
    }

    else
    {
        return fragment;
    }
}

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
    if (chromaticAberrationAmount > 0.0f)
    {
        return vec3(texture(sceneTexture, fragmentTextureCoordinate + vec2(chromaticAberrationAmount, chromaticAberrationAmount)).r, sceneTextureSampler.g, texture(sceneTexture, fragmentTextureCoordinate + vec2(-chromaticAberrationAmount, -chromaticAberrationAmount)).b);
    }
    
    else
    {
        return sceneTextureSampler;
    }
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

    //Apply bloom.
    sceneTextureSampler = ApplyBloom(sceneTextureSampler);

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