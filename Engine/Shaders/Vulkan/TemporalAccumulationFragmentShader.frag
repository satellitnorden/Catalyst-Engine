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
layout (set = 1, binding = 0) uniform sampler2D temporalAccumulationBufferMinusOne;
layout (set = 1, binding = 1) uniform sampler2D sceneTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures2Texture;

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
	bool deniedFrame = false;

	{
		//Calculate the current world position's previous perceiver position.
		vec4 previousPerceiverPosition = perceiverMatrixMinusOne * vec4(currentWorldPosition, 1.0f);

		//Calculate the current world position's previous screen coordinate.
		vec4 previousViewSpacePosition = projectionMatrixMinusOne * previousPerceiverPosition;

		//Perform perspective division.
		float previousInversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;
		previousViewSpacePosition.xy *= previousInversePerspectiveDenominator;

		vec2 previousScreenCoordinate = previousViewSpacePosition.xy * 0.5f + 0.5f;

		//Sample the previous temporal accumulation buffer.
		vec4 previousTemporalAccumulationBufferSampler = texture(temporalAccumulationBufferMinusOne, previousScreenCoordinate);

		//Calculate if this frame is denied.
		deniedFrame = deniedFrame ? true : abs(currentPerceiverPosition.z - previousPerceiverPosition.z) > 0.1f;

		float weight = float(!deniedFrame);

		accumulatedColor += previousTemporalAccumulationBufferSampler.rgb * weight;
		accumulatedWeight += weight;
	}

	//Write the current temporal accumulation.
	currentTemporalAccumulationBuffer = vec4(sceneTextureSampler.rgb, currentPerceiverPosition.z);

	//Write the result to the scene texture.
	scene = vec4(accumulatedColor / accumulatedWeight, 1.0f);
}