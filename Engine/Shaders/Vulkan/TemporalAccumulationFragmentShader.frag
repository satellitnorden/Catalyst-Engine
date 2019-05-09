//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystTemporalAccumulationCore.glsl"

//Constants.
#define TEMPORAL_ACCUMULATION_RUSSIAN_ROULETTE_CUTOFF (0.955f) //0.0025f step.

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 inverseResolution;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D previousTemporalAccumulationBuffer;
layout (set = 1, binding = 1) uniform sampler2D sceneTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures4Texture;

//Out parameters.
layout (location = 0) out vec4 currentTemporalAccumulationBuffer;
layout (location = 1) out vec4 scene;

/*
*	Returns if a coordinate is valid.
*/
bool ValidCoordinate(vec2 coordinate)
{
	return 	coordinate.x >= 0.0f
			&& coordinate.x < 1.0f
			&& coordinate.y >= 0.0f
			&& coordinate.y < 1.0f;
}

/*
*	Searches for temporal accumulation data.
*/
bool SearchForTemporalAccumulationData(vec2 coordinate, uint currentInstanceID, uint currentPrimitiveID, out vec3 previousAccumulatedColor, out AccumulationDescription previousAccumulationDescription)
{
	//Discard data randomly even if potential valid data could be found.
	if (RandomFloat(vec2(coordinate), globalRandomSeed1) >= TEMPORAL_ACCUMULATION_RUSSIAN_ROULETTE_CUTOFF)
	{
		return false;
	}

	//Keep track of whether valid data was found.
	bool foundValidData = false;

	//The sample coordinates.
	vec2 samplesCoordinates[1] = vec2[]
	(
		coordinate
	);

	//Check the surrounding fragment.
	for (int i = 0; i < 1; ++i)
	{
		AccumulationDescription sampleAccumulationDescription = UnpackAccumulationDescription(floatBitsToUint(texture(previousTemporalAccumulationBuffer, samplesCoordinates[i]).w));

		if (!foundValidData
			&& currentInstanceID == sampleAccumulationDescription.instanceID
			&& currentPrimitiveID == sampleAccumulationDescription.primitiveID
			&& ValidCoordinate(samplesCoordinates[i]))
		{
			foundValidData = true;
			previousAccumulatedColor = texture(previousTemporalAccumulationBuffer, samplesCoordinates[i]).rgb;
			previousAccumulationDescription = sampleAccumulationDescription;
		}
	}

	//Return if valid data was found.
	return foundValidData;
}

void main()
{
	//Sample the scene texture.
	vec4 sceneTextureSampler = texture(sceneTexture, fragmentTextureCoordinate);

	//Sample the scene features 2 texture.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Sample the scene features 3 texture.
	vec4 sceneFeatures4TextureSampler = texture(sceneFeatures4Texture, fragmentTextureCoordinate);

	//Calculate the world position at this fragment the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.w;

	//Now calculate it's previous screen coordinate using the previous view matrix.
	vec4 previousViewSpacePosition = previousViewMatrix * vec4(currentWorldPosition, 1.0f);

	//Perform perspective division.
	float previousInversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;
	previousViewSpacePosition.xy *= previousInversePerspectiveDenominator;

	vec2 previousScreenCoordinate = previousViewSpacePosition.xy * 0.5f + 0.5f;

	//Retrieve the current instance ID.
	uint currentInstanceID = floatBitsToUint(sceneFeatures4TextureSampler.x);

	//Retrieve the current primitive ID.
	uint currentPrimitiveID = floatBitsToUint(sceneFeatures4TextureSampler.y);

	//Search for previous temporal accumulation data.
	vec3 previousAccumulatedColor;
	AccumulationDescription previousAccumulationDescription;

	if (SearchForTemporalAccumulationData(previousScreenCoordinate, currentInstanceID, currentPrimitiveID, previousAccumulatedColor, previousAccumulationDescription))
	{
		//Write the accumulation description.
		AccumulationDescription accumulationDescription;

		accumulationDescription.accumulations = previousAccumulationDescription.accumulations + 1;
		accumulationDescription.instanceID = currentInstanceID;
		accumulationDescription.primitiveID = currentPrimitiveID;

		//Write the current temporal accumulation.
		currentTemporalAccumulationBuffer = vec4(previousAccumulatedColor + sceneTextureSampler.rgb, uintBitsToFloat(PackAccumulationDescription(accumulationDescription)));

		//Write the result to the scene texture.
		scene =  vec4((previousAccumulatedColor + sceneTextureSampler.rgb) / accumulationDescription.accumulations, 1.0f);
	}

	else
	{
		//Write the accumulation description.
		AccumulationDescription accumulationDescription;

		accumulationDescription.accumulations = 1;
		accumulationDescription.instanceID = currentInstanceID;
		accumulationDescription.primitiveID = currentPrimitiveID;

		//Write the current temporal accumulation.
   		currentTemporalAccumulationBuffer = vec4(sceneTextureSampler.rgb, uintBitsToFloat(PackAccumulationDescription(accumulationDescription)));

   		//Write the result to the scene texture. (:
		scene = vec4(sceneTextureSampler.rgb, 1.0f);
	}
}