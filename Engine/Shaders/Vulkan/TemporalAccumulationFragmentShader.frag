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
layout (set = 1, binding = 0) uniform sampler2D temporalAccumulationBufferMinusSeven;
layout (set = 1, binding = 1) uniform sampler2D temporalAccumulationBufferMinusSix;
layout (set = 1, binding = 2) uniform sampler2D temporalAccumulationBufferMinusFive;
layout (set = 1, binding = 3) uniform sampler2D temporalAccumulationBufferMinusFour;
layout (set = 1, binding = 4) uniform sampler2D temporalAccumulationBufferMinusThree;
layout (set = 1, binding = 5) uniform sampler2D temporalAccumulationBufferMinusTwo;
layout (set = 1, binding = 6) uniform sampler2D temporalAccumulationBufferMinusOne;
layout (set = 1, binding = 7) uniform sampler2D sceneTexture;
layout (set = 1, binding = 8) uniform sampler2D sceneFeatures2Texture;

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
*	Accumulates.
*/
#define ACCUMULATE(perceiverMatrixMinusN, projectionMatrixMinusN, temporalAccumulationBufferMinusN)													\
{																																					\
	vec4 previousPerceiverPosition = perceiverMatrixMinusN * vec4(currentWorldPosition, 1.0f);														\
																																					\
	vec4 previousViewSpacePosition = projectionMatrixMinusN * previousPerceiverPosition;															\
																																					\
	float previousInversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;																\
	previousViewSpacePosition.xy *= previousInversePerspectiveDenominator;																			\
																																					\
	vec2 previousScreenCoordinate = previousViewSpacePosition.xy * 0.5f + 0.5f;																		\
																																					\
	vec2 sampleCoordinates[9] = vec2[]																												\
	(																																				\
		previousScreenCoordinate + vec2(-1.0f, -1.0f) * inverseResolution,																			\
		previousScreenCoordinate + vec2(-1.0f, 0.0f) * inverseResolution,																			\
		previousScreenCoordinate + vec2(-1.0f, 1.0f) * inverseResolution,																			\
																																					\
		previousScreenCoordinate + vec2(0.0f, -1.0f) * inverseResolution,																			\
		previousScreenCoordinate + vec2(0.0f, 0.0f) * inverseResolution,																			\
		previousScreenCoordinate + vec2(0.0f, 1.0f) * inverseResolution,																			\
																																					\
		previousScreenCoordinate + vec2(1.0f, -1.0f) * inverseResolution,																			\
		previousScreenCoordinate + vec2(1.0f, 0.0f) * inverseResolution,																			\
		previousScreenCoordinate + vec2(1.0f, 1.0f) * inverseResolution																				\
	);																																				\
																																					\
	float closestPreviousTemporalAccumulationBufferDepth = 9999999.0f;																				\
	vec3 closestPreviousTemporalAccumulationBufferColor;																							\
																																					\
	for (int i = 0; i < 9; ++i)																														\
	{																																				\
		vec4 previousTemporalAccumulationBufferSampler = texture(temporalAccumulationBufferMinusN, sampleCoordinates[i]);							\
		float previousTemporalAccumulationBufferDepth = abs(previousPerceiverPosition.z - previousTemporalAccumulationBufferSampler.w);				\
																																					\
		if (closestPreviousTemporalAccumulationBufferDepth > previousTemporalAccumulationBufferDepth)												\
		{																																			\
			closestPreviousTemporalAccumulationBufferDepth = previousTemporalAccumulationBufferDepth;												\
			closestPreviousTemporalAccumulationBufferColor = previousTemporalAccumulationBufferSampler.rgb;											\
		}																																			\
	}																																				\
																																					\
	float weight = 1.0f;																															\
																																					\
	weight *= float(ValidCoordinate(previousScreenCoordinate));																						\
	weight *= pow(1.0f - min(closestPreviousTemporalAccumulationBufferDepth, 1.0f), 2.0f);															\
	weight *= 1.0f - min(abs(CalculateAverage(sceneTextureSampler.rgb) - CalculateAverage(closestPreviousTemporalAccumulationBufferColor)), 1.0f);	\
																																					\
	weight = pow(weight, 2.0f);																														\
																																					\
	accumulatedColor += closestPreviousTemporalAccumulationBufferColor * weight;																	\
	accumulatedWeight += weight;																													\
}

void main()
{
	//Sample the scene texture.
	vec4 sceneTextureSampler = texture(sceneTexture, fragmentTextureCoordinate);

	//Sample the scene features 2 texture.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Calculate the world position at this fragment for the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.w;

	//Calculate it's current perceiver position.
	vec4 currentPerceiverPosition = perceiverMatrix * vec4(currentWorldPosition, 1.0f);

	//Add all accumulated colors.
	vec3 accumulatedColor = sceneTextureSampler.rgb;
	float accumulatedWeight = 1.0f;

	ACCUMULATE(perceiverMatrixMinusOne, projectionMatrixMinusOne, temporalAccumulationBufferMinusOne);
	ACCUMULATE(perceiverMatrixMinusTwo, projectionMatrixMinusTwo, temporalAccumulationBufferMinusTwo);
	ACCUMULATE(perceiverMatrixMinusThree, projectionMatrixMinusThree, temporalAccumulationBufferMinusThree);
	ACCUMULATE(perceiverMatrixMinusFour, projectionMatrixMinusFour, temporalAccumulationBufferMinusFour);
	ACCUMULATE(perceiverMatrixMinusFive, projectionMatrixMinusFive, temporalAccumulationBufferMinusFive);
	ACCUMULATE(perceiverMatrixMinusSix, projectionMatrixMinusSix, temporalAccumulationBufferMinusSix);
	ACCUMULATE(perceiverMatrixMinusSeven, projectionMatrixMinusSeven, temporalAccumulationBufferMinusSeven);

	//Write the current temporal accumulation.
	currentTemporalAccumulationBuffer = vec4(sceneTextureSampler.rgb, currentPerceiverPosition.z);

	//Write the result to the scene texture.
	scene = vec4(accumulatedColor / accumulatedWeight, 1.0f);
}