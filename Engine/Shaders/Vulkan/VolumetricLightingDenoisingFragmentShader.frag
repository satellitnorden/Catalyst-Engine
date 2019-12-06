//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define VOLUMETRIC_LIGHTING_DENOISING_SIZE (10.0f)
#define VOLUMETRIC_LIGHTING_DENOISING_START_END (VOLUMETRIC_LIGHTING_DENOISING_SIZE * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	float depth;
};

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
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D volumetricLightingTexture;

//Out parameters.
layout (location = 0) out vec4 volumetricLighting;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.depth = LinearizeDepth(sceneFeatures2.w);

	return features;
}

void main()
{
	//Sample the scene features and volumetric lighting at the current fragment.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);
	vec3 currentVolumetricLighting = texture(volumetricLightingTexture, fragmentTextureCoordinate).rgb;

	//Calculate the start/end.
	float startEnd = VOLUMETRIC_LIGHTING_DENOISING_START_END * stride;

	//Sample neighboring fragments.
	vec3 denoisedVolumetricLighting = vec3(0.0f);
	float volumetricLightingWeightSum = 0.0f;

	for (float x = -startEnd; x <= startEnd; ++x)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

		vec3 sampleVolumetricLighting = texture(volumetricLightingTexture, sampleCoordinate).rgb;
		SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. Is the sample coordinate valid?
		*	2. How closely aligned are the hit distances to each other?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= float(ValidCoordinate(sampleCoordinate));
		//sampleWeight *= 1.0f - min(abs(currentFeatures.depth - sampleFeatures.depth), 1.0f);

		denoisedVolumetricLighting += sampleVolumetricLighting * sampleWeight;
		volumetricLightingWeightSum += sampleWeight;
	}

	//Normalize the denoised volumetric lighting.
	denoisedVolumetricLighting = volumetricLightingWeightSum == 0.0f ? currentVolumetricLighting : denoisedVolumetricLighting / volumetricLightingWeightSum;

	//Write the fragment.
	volumetricLighting = vec4(denoisedVolumetricLighting, 1.0f);
}