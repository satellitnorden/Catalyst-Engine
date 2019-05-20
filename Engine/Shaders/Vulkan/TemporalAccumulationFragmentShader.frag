//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

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
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.z;

	//Calculate the previous hit distance.
	float previousHitDistance = length(currentWorldPosition - perceiverWorldPositionMinusOne);

	//Calculate the previous screen coordinate.
	vec4 previousViewSpacePosition = viewMatrixMinusOne * vec4(currentWorldPosition, 1.0f);
	vec2 previousScreenCoordinate = 0.5f * (previousViewSpacePosition.xy / previousViewSpacePosition.w) + 0.5f;

	//Sample the previous temporal accumulation description buffer.
	vec4 closestPreviousTemporalAccumulationColorBuffer = texture(temporalAccumulationColorBufferMinusOne, previousScreenCoordinate);
	vec4 closestPreviousTemporalAccumulationDescriptionBuffer = texture(temporalAccumulationDescriptionBufferMinusOne, previousScreenCoordinate);

	//Retrieve the total accumulations.
	float totalAccumulations = closestPreviousTemporalAccumulationColorBuffer.w + 1.0f;

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How close were the hit distances?
	*	3. How fast is the perceiver moving?
	*	4. How fast is the perceiver rotating?
	*/
	float weight = 1.0f;

	weight *= float(ValidCoordinate(previousScreenCoordinate));
	weight *= pow(max(1.0f - abs(previousHitDistance - closestPreviousTemporalAccumulationDescriptionBuffer.x), 0.0f), 2.0f);
	weight *= max(1.0f - length(perceiverVelocity), 0.0f);
	weight *= max(perceiverRotationVelocity, 0.0f);

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
	currentTemporalAccumulationDescriptionBuffer = vec4(sceneFeatures2TextureSampler.z, 0.0f, 0.0f, 0.0f);

	//Write the result to the scene texture.
	scene = vec4(blendedAverage, 1.0f);
}