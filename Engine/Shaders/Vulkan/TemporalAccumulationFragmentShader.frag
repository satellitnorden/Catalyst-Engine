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
layout (set = 1, binding = 4) uniform sampler2D sceneFeatures4Texture;

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

	//Sample the scene features 4 texture.
	vec4 sceneFeatures4TextureSampler = texture(sceneFeatures4Texture, fragmentTextureCoordinate);

	//Unpack the current instance ID.
	int currentInstanceID = floatBitsToInt(sceneFeatures4TextureSampler.x);

	//Unpack the current primitive ID.
	int currentPrimitiveID = floatBitsToInt(sceneFeatures4TextureSampler.y);

	//Calculate the current barycentric coordinates.
	vec3 currentBarycentricCoordinates = vec3(1.0f - sceneFeatures4TextureSampler.z - sceneFeatures4TextureSampler.w, sceneFeatures4TextureSampler.z, sceneFeatures4TextureSampler.w);

	//Calculate the world position at this fragment for the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.z;

	//Calculate the current perceiver position.
	vec4 currentPerceiverPosition = perceiverMatrix * vec4(currentWorldPosition, 1.0f);

	//Calculate the previous perceiver position.
	vec4 previousPerceiverPosition = perceiverMatrixMinusOne * vec4(currentWorldPosition, 1.0f);

	//Calculate the previous screen coordinate.
	vec4 previousViewSpacePosition = projectionMatrixMinusOne * previousPerceiverPosition;

	float previousInversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;
	previousViewSpacePosition.xy *= previousInversePerspectiveDenominator;

	vec2 previousScreenCoordinate = previousViewSpacePosition.xy * 0.5f + 0.5f;

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

	float closestPreviousTemporalAccumulationBufferDistance = 1.0f;
	vec4 closestPreviousTemporalAccumulationColorBuffer;
	vec4 closestPreviousTemporalAccumulationDescriptionBuffer;

	for (int i = 0; i < 9; ++i)
	{
		//Sample the previous temporal accumulation description buffer.
		vec4 samplePreviousTemporalAccumulationDescriptionBuffer = texture(temporalAccumulationDescriptionBufferMinusOne, sampleCoordinates[i]);

		//Unpack the previous sample instance ID.
		int samplePreviousInstanceID = floatBitsToInt(samplePreviousTemporalAccumulationDescriptionBuffer.x);

		//Unpack the previous sample primitive ID.
		int samplePreviousPrimitiveID = floatBitsToInt(samplePreviousTemporalAccumulationDescriptionBuffer.y);

		//Calculate the previous sample barycentric coordinates.
		vec3 samplePreviousBarycentricCoordinates = vec3(1.0f - samplePreviousTemporalAccumulationDescriptionBuffer.z - samplePreviousTemporalAccumulationDescriptionBuffer.w, samplePreviousTemporalAccumulationDescriptionBuffer.z, samplePreviousTemporalAccumulationDescriptionBuffer.w);

		//Calculate the previous sample distance.
		float previousSampleDistance = LengthSquared3(currentBarycentricCoordinates - samplePreviousBarycentricCoordinates);

		if (currentInstanceID == samplePreviousInstanceID
			&& currentPrimitiveID == samplePreviousPrimitiveID
			&& closestPreviousTemporalAccumulationBufferDistance > previousSampleDistance)
		{
			closestPreviousTemporalAccumulationBufferDistance = previousSampleDistance;
			closestPreviousTemporalAccumulationColorBuffer = texture(temporalAccumulationColorBufferMinusOne, sampleCoordinates[i]);
			closestPreviousTemporalAccumulationDescriptionBuffer = samplePreviousTemporalAccumulationDescriptionBuffer;
		}
	}

	//Retrieve the total accumulations.
	float totalAccumulations = closestPreviousTemporalAccumulationColorBuffer.w + 1.0f;

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Was the sample outside the screen? If so, it's' invalid.
	*	2. How close to the current barycentric coordinate was the sample?
	*	3. How fast is the perceiver moving?
	*	4. How fast is the perceiver rotating?
	*	5. To prioritize newer samples, decrease the weight as total accumulations increases.
	*/
	float weight = 1.0f;

	weight *= float(ValidCoordinate(previousScreenCoordinate));
	weight *= pow(max(1.0f - closestPreviousTemporalAccumulationBufferDistance, 0.0f), 16.0f);
	weight *= max(1.0f - length(perceiverVelocity), 0.0f);
	weight *= max(perceiverRotationVelocity, 0.0f);
	weight *= max(1.0f - (totalAccumulations / 4096.0f), 0.0f);

	//Calculate the previous average.
	vec3 previousAverage = closestPreviousTemporalAccumulationColorBuffer.rgb;

	//Calculate the smoothing constant.
	float smoothingConstant = 2.0f / (totalAccumulations + 1.0f);

	//Calculate the exponentially moving average.
	vec3 exponentiallyMovingAverage = (sceneTextureSampler.rgb - previousAverage) * smoothingConstant + previousAverage;

	//Calculate the blended average.
	vec3 blendedAverage = mix(sceneTextureSampler.rgb, exponentiallyMovingAverage, weight);

	//Write the current temporal accumulation color buffer.
	currentTemporalAccumulationColorBuffer = vec4(blendedAverage, mix(1.0f, totalAccumulations, weight));

	//Write the current temporal accumulation description buffer.
	currentTemporalAccumulationDescriptionBuffer = vec4(intBitsToFloat(currentInstanceID), intBitsToFloat(currentPrimitiveID), currentBarycentricCoordinates.y, currentBarycentricCoordinates.z);

	//Write the result to the scene texture.
	scene = vec4(blendedAverage, 1.0f);
}