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
layout (set = 1, binding = 0) uniform sampler2D currentTemporalAccumulationDescriptionBufferTexture;
layout (set = 1, binding = 1) uniform sampler2D temporalAccumulationIndirectLightingColorBufferMinusOne;
layout (set = 1, binding = 2) uniform sampler2D temporalAccumulationDirectLightingColorBufferMinusOne;
layout (set = 1, binding = 3) uniform sampler2D indirectLightingTexture;
layout (set = 1, binding = 4) uniform sampler2D directLightingTexture;
layout (set = 1, binding = 5) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 currentTemporalAccumulationIndirectLightingColorBuffer;
layout (location = 1) out vec4 currentTemporalAccumulationDirectLightingColorBuffer;
layout (location = 2) out vec4 currentTemporalAccumulationDescriptionBuffer;
layout (location = 3) out vec4 indirectLighting;
layout (location = 4) out vec4 directLighting;

void main()
{
	//Sample the current temporal accumulation description buffer.
	vec4 currentTemporalAccumulationDescriptionBufferTextureSampler = texture(currentTemporalAccumulationDescriptionBufferTexture, fragmentTextureCoordinate);

	//Sample the indirect lighting texture.
	vec4 indirectLightingTextureSampler = texture(indirectLightingTexture, fragmentTextureCoordinate);

	//Sample the direct lighting texture.
	vec4 directLightingTextureSampler = texture(directLightingTexture, fragmentTextureCoordinate);

	//Sample the scene features 2 texture.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Calculate the world position at this fragment for the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.z;

	//Calculate the previous screen coordinate.
	vec4 previousViewSpacePosition = viewMatrixMinusOne * vec4(currentWorldPosition, 1.0f);
	vec2 previousScreenCoordinate = 0.5f * (previousViewSpacePosition.xy / previousViewSpacePosition.w) + 0.5f;

	//Retrieve the weight.
	float weight = currentTemporalAccumulationDescriptionBufferTextureSampler.y;

	//Retrieve the total accumulations.
	float totalAccumulations = currentTemporalAccumulationDescriptionBufferTextureSampler.z;

	//Write the current temporal accumulation description buffer.
	currentTemporalAccumulationDescriptionBuffer = vec4(length(currentWorldPosition - perceiverWorldPosition), weight, mix(1.0f, totalAccumulations, weight), 0.0f);

	//Sample the previous temporal accumulation color buffers.
	vec4 closestPreviousTemporalAccumulationIndirectLightingColorBuffer = texture(temporalAccumulationIndirectLightingColorBufferMinusOne, previousScreenCoordinate);
	vec4 closestPreviousTemporalAccumulationDirectLightingColorBuffer = texture(temporalAccumulationDirectLightingColorBufferMinusOne, previousScreenCoordinate);

	{
		//Retrieve the previous average.
		vec3 previousAverage = closestPreviousTemporalAccumulationIndirectLightingColorBuffer.rgb;

		//Calculate the smoothing constant.
		float smoothingConstant = 1.0f / (totalAccumulations + 1.0f);

		//Calculate the exponentially moving average.
		vec3 exponentiallyMovingAverage = (indirectLightingTextureSampler.rgb - previousAverage) * smoothingConstant + previousAverage;

		//Calculate the blended average.
		vec3 blendedAverage = mix(indirectLightingTextureSampler.rgb, exponentiallyMovingAverage, weight);

		//Write the current temporal accumulation color buffer.
		currentTemporalAccumulationIndirectLightingColorBuffer = vec4(blendedAverage, 1.0f);

		//Write the result to the indirect lighting texture.
		indirectLighting = vec4(blendedAverage, 1.0f);
	}

	{
		//Retrieve the previous average.
		vec3 previousAverage = closestPreviousTemporalAccumulationDirectLightingColorBuffer.rgb;

		//Calculate the smoothing constant.
		float smoothingConstant = 1.0f / (totalAccumulations + 1.0f);

		//Calculate the exponentially moving average.
		vec3 exponentiallyMovingAverage = (directLightingTextureSampler.rgb - previousAverage) * smoothingConstant + previousAverage;

		//Calculate the blended average.
		vec3 blendedAverage = mix(directLightingTextureSampler.rgb, exponentiallyMovingAverage, weight);

		//Write the current temporal accumulation color buffer.
		currentTemporalAccumulationDirectLightingColorBuffer = vec4(blendedAverage, 1.0f);

		//Write the result to the direct lighting texture.
		directLighting = vec4(blendedAverage, 1.0f);
	}
}