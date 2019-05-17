//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DIRECT_LIGHTING_DENOISING_SIZE (15)
#define DIRECT_LIGHTING_DENOISING_START_END ((DIRECT_LIGHTING_DENOISING_SIZE - 1) / 2)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 hitPosition;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 direction;
};

//Descriptor set data.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D lightVisibilityTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.z;

	return features;
}

void main()
{
	//Sample the scene features at the current fragment.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Denoise the visibility term in the direct lighting result.
	float denoisedVisibility = 0.0f;
	float denoisedVisibilityTotalWeight = 0.0f;

	for (float x = -DIRECT_LIGHTING_DENOISING_START_END; x <= DIRECT_LIGHTING_DENOISING_START_END; ++x)
	{
		//Calculate the sample coordinate.
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;
		SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. How closely aligned are the hit position?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= pow(1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f), 4.0f);

		denoisedVisibility += texture(lightVisibilityTexture, sampleCoordinate).x * sampleWeight;

		denoisedVisibilityTotalWeight += sampleWeight;
	}

	//Normalize the denoised visibility.
	denoisedVisibility /= denoisedVisibilityTotalWeight;

	//Write the fragment.
	fragment = vec4(denoisedVisibility);
}