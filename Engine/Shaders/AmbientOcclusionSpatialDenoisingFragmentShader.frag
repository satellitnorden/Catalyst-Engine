//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	float view_distance;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 INVERSE_RESOLUTION;
	layout (offset = 8) int STRIDE;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D ambientOcclusionTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.view_distance = CalculateViewSpacePosition(fragmentTextureCoordinate, sceneFeatures2.w).z;

	return features;
}

void main()
{
	//Sample the ambient occlusion features at the current fragment.
	vec3 currentAmbientOcclusion = texture(ambientOcclusionTexture, fragmentTextureCoordinate).rgb;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample neighboring fragments.
	vec3 denoisedAmbientOcclusion = vec3(0.0f);
	float ambientOcclusionWeightSum = 0.0f;

	for (int y = -STRIDE; y <= STRIDE; y += STRIDE)
	{
		for (int x = -STRIDE; x <= STRIDE; x += STRIDE)
		{
			vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(float(x), float(y)) * INVERSE_RESOLUTION;

			vec3 sampleAmbientOcclusion = texture(ambientOcclusionTexture, sampleCoordinate).rgb;
			SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. Is the sample coordinate valid?
			*	2. How closely aligned are the depths to each other?
			*/
			float sampleWeight = 1.0f;

			sampleWeight *= float(ValidCoordinate(sampleCoordinate));
			sampleWeight *= pow(1.0f - min(abs(currentFeatures.view_distance - sampleFeatures.view_distance), 1.0f), 2.0f);

			denoisedAmbientOcclusion += sampleAmbientOcclusion * sampleWeight;
			ambientOcclusionWeightSum += sampleWeight;
		}
	}
	
	//Normalize the denoised ambient occlusion. 
	denoisedAmbientOcclusion = ambientOcclusionWeightSum == 0.0f ? currentAmbientOcclusion : denoisedAmbientOcclusion / ambientOcclusionWeightSum;

	//Write the fragment.
	fragment = vec4(denoisedAmbientOcclusion, 1.0f);
}