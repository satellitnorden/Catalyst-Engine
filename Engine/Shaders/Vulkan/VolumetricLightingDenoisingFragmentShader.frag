//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DENOISING_SIZE (8.0f)
#define DENOISING_START_END (DENOISING_SIZE * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 direction;
	layout (offset = 8) float stride;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D volumetricLightingTexture;

//Out parameters.
layout (location = 0) out vec4 diffuseIrradiance;

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
	//Sample the volumetric lighting at the current fragment.
	vec3 currentVolumetricLighting = texture(volumetricLightingTexture, fragmentTextureCoordinate).rgb;

	//Calculate the start/end.
	float startAndEnd = DENOISING_START_END * stride;

	//Sample neighboring fragments.
	vec3 denoisedVolumetricLighting = vec3(0.0f);
	float volumetricLightingWeightSum = 0.0f;

	for (float x = -startAndEnd; x <= startAndEnd; x += stride)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

		vec3 sampleVolumetricLighting = texture(volumetricLightingTexture, sampleCoordinate).rgb;

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. Is the sample coordinate valid?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= float(ValidCoordinate(sampleCoordinate));

		denoisedVolumetricLighting += sampleVolumetricLighting * sampleWeight;
		volumetricLightingWeightSum += sampleWeight;
	}

	//Normalize the denoised volumetric lighting.
	denoisedVolumetricLighting = volumetricLightingWeightSum == 0.0f ? currentVolumetricLighting : denoisedVolumetricLighting / volumetricLightingWeightSum;

	//Write the fragment.
	diffuseIrradiance = vec4(denoisedVolumetricLighting, 1.0f);
}