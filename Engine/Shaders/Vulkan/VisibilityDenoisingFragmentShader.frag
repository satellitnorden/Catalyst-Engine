//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define VISIBILITY_DENOISING_SIZE (16.0f)
#define VISIBILITY_DENOISING_START_END (VISIBILITY_DENOISING_SIZE * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 hitPosition;
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
layout (set = 1, binding = 1) uniform sampler2D visibilityTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.w;

	return features;
}

void main()
{
	//Sample the visibility and scene features at the current fragment.
	float currentVisibility = texture(visibilityTexture, fragmentTextureCoordinate).x;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Calculate the start/end.
	float startAndEnd = VISIBILITY_DENOISING_START_END * stride;

	//Sample neighboring fragments.
	float denoisedVisibility = 0.0f;
	float visibilityWeightSum = 0.0f;

	for (float x = -startAndEnd; x <= startAndEnd; x += stride)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

		float sampleVisibility = texture(visibilityTexture, sampleCoordinate).x;
		SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. How closely aligned are the hit positions to each other?
		*	2. Is the sample coordinate valid?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= pow(1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f), 2.0f);
		sampleWeight *= float(ValidCoordinate(sampleCoordinate));

		denoisedVisibility += sampleVisibility * sampleWeight;
		visibilityWeightSum += sampleWeight;
	}

	//Normalize the denoised diffuse irradiance. 
	denoisedVisibility = visibilityWeightSum == 0.0f ? currentVisibility : denoisedVisibility / visibilityWeightSum;

	//Write the fragment.
	fragment = vec4(denoisedVisibility, 0.0f, 0.0f, 1.0f);
}