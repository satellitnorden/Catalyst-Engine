//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define TEMPORAL_ACCUMULATION_ACCEPTED_DEPTH (0.01f)

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
layout (set = 1, binding = 0) uniform sampler2D temporalAccumulationColorBufferMinusOne;
layout (set = 1, binding = 1) uniform sampler2D temporalAccumulationDescriptionBufferMinusOne;
layout (set = 1, binding = 2) uniform sampler2D sceneTexture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 currentTemporalAccumulationColorBuffer;
layout (location = 1) out vec4 currentTemporalAccumulationDescriptionBuffer;
layout (location = 2) out vec4 scene;

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

void main()
{
	//Sample the scene texture.
	vec4 sceneTextureSampler = texture(sceneTexture, fragmentTextureCoordinate);

	//Sample the scene features 2 texture.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Calculate the world position at this fragment for the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.w;

	//Calculate the current perceiver position.
	vec4 currentPerceiverPosition = perceiverMatrix * vec4(currentWorldPosition, 1.0f);

	//Calculate the previous perceiver position.
	vec4 previousPerceiverPosition = perceiverMatrixMinusOne * vec4(currentWorldPosition, 1.0f);

	//Calculate the previous screen coordinate.
	vec4 previousViewSpacePosition = projectionMatrixMinusOne * previousPerceiverPosition;

	float previousInversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;
	previousViewSpacePosition.xy *= previousInversePerspectiveDenominator;

	vec2 previousScreenCoordinate = previousViewSpacePosition.xy * 0.5f + 0.5f;

	//Find the closest sample, depth-wise.
	vec2 sampleCoordinates[9] = vec2[]
	(
		previousScreenCoordinate + vec2(-1.0f, -1.0f) * inverseResolution,
		previousScreenCoordinate + vec2(-1.0f, 0.0f) * inverseResolution,
		previousScreenCoordinate + vec2(-1.0f, 1.0f) * inverseResolution,

		previousScreenCoordinate + vec2(0.0f, -1.0f) * inverseResolution,
		previousScreenCoordinate + vec2(0.0f, 0.0f) * inverseResolution,
		previousScreenCoordinate + vec2(0.0f, 1.0f) * inverseResolution,

		previousScreenCoordinate + vec2(1.0f, -1.0f) * inverseResolution,
		previousScreenCoordinate + vec2(1.0f, 0.0f) * inverseResolution,
		previousScreenCoordinate + vec2(1.0f, 1.0f) * inverseResolution
	);
	
	vec4 closestPreviousTemporalAccumulationColorBuffer;
	vec4 closestPreviousTemporalAccumulationDescriptionBuffer;
	float closestPreviousTemporalAccumulationBufferDepth = 9999999.0f;

	for (int i = 0; i < 9; ++i)
	{
		vec4 previousTemporalAccumulationDescriptionBufferSampler = texture(temporalAccumulationDescriptionBufferMinusOne, sampleCoordinates[i]);
		float previousTemporalAccumulationBufferDepth = abs(previousPerceiverPosition.z - previousTemporalAccumulationDescriptionBufferSampler.x);

		if (closestPreviousTemporalAccumulationBufferDepth > previousTemporalAccumulationBufferDepth)
		{
			closestPreviousTemporalAccumulationBufferDepth = previousTemporalAccumulationBufferDepth;
			closestPreviousTemporalAccumulationColorBuffer = texture(temporalAccumulationColorBufferMinusOne, sampleCoordinates[i]);
			closestPreviousTemporalAccumulationDescriptionBuffer = previousTemporalAccumulationDescriptionBufferSampler;
		}
	}
	
	//Calculate the weight of the previous temporal accumulation color.
	float weight = 1.0f;
	
	weight *= float(ValidCoordinate(previousScreenCoordinate));
	weight *= pow(1.0f - min(closestPreviousTemporalAccumulationBufferDepth, 1.0f), 16.0f);
	weight *= pow(1.0f - min(abs(CalculateAverage(sceneTextureSampler.rgb) - CalculateAverage(closestPreviousTemporalAccumulationColorBuffer.rgb)), 1.0f), 4.0f);

	//Calculate the ideal average.
	float idealAverage = closestPreviousTemporalAccumulationDescriptionBuffer.y + ((1.0f - closestPreviousTemporalAccumulationDescriptionBuffer.y) * 0.25f);

	//Calculate the current average.
	float currentAverage = idealAverage * weight;

	//Calculate the accumulated scene color.
	vec3 accumulatedSceneColor = mix(sceneTextureSampler.rgb, closestPreviousTemporalAccumulationColorBuffer.rgb, currentAverage);

	//Write the current temporal accumulation color buffer.
	currentTemporalAccumulationColorBuffer = vec4(accumulatedSceneColor, 1.0f);

	//Write the current temporal accumulation description buffer.
	currentTemporalAccumulationDescriptionBuffer = vec4(currentPerceiverPosition.z, currentAverage, 0.0f, 0.0f);

	//Write the result to the scene texture.
	scene = vec4(accumulatedSceneColor, 1.0f);
}