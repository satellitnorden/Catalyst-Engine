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
    layout (offset = 1872) float windSpeed; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//Post processing data.
layout (push_constant) uniform PostProcessingData
{
    float saturation;
};

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

/*
*	Calculates the average of a fragment.
*/
float CalculateAverage(vec3 fragment)
{
	return fragment.r * 0.2126f + fragment.g * 0.7152f + fragment.b * 0.0722f;
}

/*
*   Applies saturation.
*/
vec3 ApplySaturation(vec3 sceneTextureSampler)
{
    if (saturation != 0.0f)
    {
        //Calculate the average.
        float average = CalculateAverage(sceneTextureSampler);

        //Calculate the grayscale color.
        vec3 grayscaleColor = vec3(average, average, average);

        //Return the calculated color.
        return mix (grayscaleColor, sceneTextureSampler, saturation);
    }
    
    else
    {
        return sceneTextureSampler;
    }
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

/*
*	Applies FXAA.
*/
vec3 ApplyFXAA(vec3 fragment)
{
	//Sample the 8 surrounding pixels.
	#define WIDTH_OFFSET ((1.0f / 1920.0f) * 1.5f)
	#define HEIGHT_OFFSET ((1.0f / 1080.0f) * 1.5f)

	vec3 samples[9];

	samples[0] = texture(sceneTexture, fragmentTextureCoordinate + vec2(-WIDTH_OFFSET, -HEIGHT_OFFSET)).rgb;
	samples[1] = texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, -HEIGHT_OFFSET)).rgb;
	samples[2] = texture(sceneTexture, fragmentTextureCoordinate + vec2(WIDTH_OFFSET, -HEIGHT_OFFSET)).rgb;
	samples[3] = texture(sceneTexture, fragmentTextureCoordinate + vec2(-WIDTH_OFFSET, 0.0f)).rgb;
	samples[4] = fragment;
	samples[5] = texture(sceneTexture, fragmentTextureCoordinate + vec2(WIDTH_OFFSET, 0.0f)).rgb;
	samples[6] = texture(sceneTexture, fragmentTextureCoordinate + vec2(-WIDTH_OFFSET, HEIGHT_OFFSET)).rgb;
	samples[7] = texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, HEIGHT_OFFSET)).rgb;
	samples[8] = texture(sceneTexture, fragmentTextureCoordinate + vec2(WIDTH_OFFSET, HEIGHT_OFFSET)).rgb;

	//Calculate the averages for all 9 samples.
	float averages[9];

	for (int i = 0; i < 9; ++i)
	{
		averages[i] = CalculateAverage(samples[i]);
	}

	//Determine the minimum and the maximum average.
	float minimumAverage = averages[0];
	float maximumAverage = averages[0];

	for (int i = 1; i < 9; ++i)
	{
		minimumAverage = min(minimumAverage, averages[i]);
		maximumAverage = max(maximumAverage, averages[i]);
	}

	//Calculate the average range.
	float averageRange = clamp(maximumAverage - minimumAverage, 0.0f, 1.0f);

	//Calculate the blurred fragment.
	vec3 blurredFragment =	samples[0] * 0.0625f + 
							samples[1] * 0.125f + 
							samples[2] * 0.0625f + 
							samples[3] * 0.125f + 
							samples[4] * 0.25f + 
							samples[5] * 0.125f + 
							samples[6] * 0.0625f + 
							samples[7] * 0.125f + 
							samples[8] * 0.0625f;

	//Blend between the original fragment and the blurred fragment depending on the average range.
	return mix(fragment, blurredFragment, averageRange);
}

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureSampler = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply saturation.
    sceneTextureSampler = ApplySaturation(sceneTextureSampler);

    //Apply HDR.
    sceneTextureSampler = ApplyHDR(sceneTextureSampler);

    //Apply gamma correction.
    sceneTextureSampler = ApplyGammaCorrection(sceneTextureSampler);

    //Apply FXAA.
    sceneTextureSampler = ApplyFXAA(sceneTextureSampler);

    //Set the fragment color.
    fragmentColor = vec4(sceneTextureSampler, 1.0f);
}