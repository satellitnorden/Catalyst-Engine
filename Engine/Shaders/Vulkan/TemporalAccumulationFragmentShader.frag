//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystTemporalAccumulationCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D previousTemporalAccumulationBuffer;
layout (set = 1, binding = 1) uniform sampler2D sceneTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures3Texture;

//Out parameters.
layout (location = 0) out vec4 temporalAccumulation;

void main()
{
	//Sample the scene texture.
	vec4 sceneTextureSampler = texture(sceneTexture, fragmentTextureCoordinate);

	//Sample the scene features 2 texture.
	vec4 sceneFeatures1TextureSampler = texture(sceneFeatures1Texture, fragmentTextureCoordinate);

	//Sample the scene features 3 texture.
	vec4 sceneFeatures3TextureSampler = texture(sceneFeatures3Texture, fragmentTextureCoordinate);

	//Calculate the world position at this fragment the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures1TextureSampler.w;

	//Now calculate it's previous screen coordinate using the previous view matrix.
	vec4 previousViewSpacePosition = previousViewMatrix * vec4(currentWorldPosition, 1.0f);

	//Perform perspective division.
	float previousInversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;
	previousViewSpacePosition.xy *= previousInversePerspectiveDenominator;

	vec2 previousScreenCoordinate = previousViewSpacePosition.xy * 0.5f + 0.5f;

	//Retrieve the current instance ID.
	uint currentInstanceID = floatBitsToUint(sceneFeatures3TextureSampler.x);

	//Retrieve the current primitive ID.
	uint currentPrimitiveID = floatBitsToUint(sceneFeatures3TextureSampler.y);

	//Sample the previous temporal accumulation buffer.
	vec4 previousTemporalAccumulationBufferSampler = texture(previousTemporalAccumulationBuffer, previousScreenCoordinate);

	//Unpack the previous accumulation description.
	AccumulationDescription previousAccumulationDescription = UnpackAccumulationDescription(floatBitsToUint(previousTemporalAccumulationBufferSampler.w));

	if (currentInstanceID == previousAccumulationDescription.instanceID
		&& currentPrimitiveID == previousAccumulationDescription.primitiveID
		&& RandomFloat(fragmentTextureCoordinate, globalRandomSeed1) < 0.999f)
	{
		//Write the accumulation description.
		AccumulationDescription accumulationDescription;

		accumulationDescription.accumulations = previousAccumulationDescription.accumulations + 1;
		accumulationDescription.instanceID = currentInstanceID;
		accumulationDescription.primitiveID = currentPrimitiveID;

		//Write the temporal accumulation.
   		temporalAccumulation = vec4(previousTemporalAccumulationBufferSampler.rgb + sceneTextureSampler.rgb, uintBitsToFloat(PackAccumulationDescription(accumulationDescription)));
	}

	else
	{
		//Write the accumulation description.
		AccumulationDescription accumulationDescription;

		accumulationDescription.accumulations = 1;
		accumulationDescription.instanceID = currentInstanceID;
		accumulationDescription.primitiveID = currentPrimitiveID;

		//Write the temporal accumulation.
   		temporalAccumulation = vec4(sceneTextureSampler.rgb, uintBitsToFloat(PackAccumulationDescription(accumulationDescription)));
	}
}