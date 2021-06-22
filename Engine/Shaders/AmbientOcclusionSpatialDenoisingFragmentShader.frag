//Includes.
#include "CatalystPackingUtilities.glsl"
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
layout (set = 1, binding = 0) uniform sampler2D SOURCE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

	SceneFeatures features;

	features.view_distance = CalculateViewSpacePosition(fragmentTextureCoordinate, sceneFeatures2.w).z;

	return features;
}

void CatalystShaderMain()
{
	//Sample the ambient occlusion features at the current fragment.
	vec3 currentAmbientOcclusion = texture(SOURCE_TEXTURE, fragmentTextureCoordinate).rgb;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample neighboring fragments.
	vec3 denoisedAmbientOcclusion = vec3(0.0f);
	float ambientOcclusionWeightSum = 0.0f;

	for (int y = -STRIDE; y <= STRIDE; y += STRIDE)
	{
		for (int x = -STRIDE; x <= STRIDE; x += STRIDE)
		{
			vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(float(x), float(y)) * INVERSE_RESOLUTION;

			vec3 sampleAmbientOcclusion = texture(SOURCE_TEXTURE, sampleCoordinate).rgb;
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