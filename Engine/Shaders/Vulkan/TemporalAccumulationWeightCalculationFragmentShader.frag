//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D temporalAccumulationDescriptionBufferMinusOne;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 currentTemporalAccumulationDescriptionBuffer;

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
	//Sample the scene features 2 texture.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Calculate the world position at this fragment for the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.w;

	//Calculate the previous hit distance.
	float previousHitDistance = length(currentWorldPosition - perceiverWorldPositionMinusOne);

	//Calculate the previous screen coordinate.
	vec4 previousViewSpacePosition = viewMatrixMinusOne * vec4(currentWorldPosition, 1.0f);
	vec2 previousScreenCoordinate = 0.5f * (previousViewSpacePosition.xy / previousViewSpacePosition.w) + 0.5f;

	//Sample the previous temporal accumulation description buffer.
	vec4 closestPreviousTemporalAccumulationDescriptionBuffer = texture(temporalAccumulationDescriptionBufferMinusOne, previousScreenCoordinate);

	//Retrieve the total accumulations.
	float totalAccumulations = closestPreviousTemporalAccumulationDescriptionBuffer.z + 1.0f;

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How close were the hit distances?
	*	3. Decrase weight over time as more accumulations are made.
	*/
	float weight = 1.0f;

	weight *= float(ValidCoordinate(previousScreenCoordinate));
	weight *= pow(max(1.0f - abs(previousHitDistance - closestPreviousTemporalAccumulationDescriptionBuffer.x), 0.0f), 32.0f);
	weight *= 1.0f - min(totalAccumulations / 512.0f, 1.0f);

	//Write the current temporal accumulation description buffer.
	currentTemporalAccumulationDescriptionBuffer = vec4(length(currentWorldPosition - perceiverWorldPosition), weight, totalAccumulations, 0.0f);
}