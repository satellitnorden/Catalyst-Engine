//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define INDIRECT_LIGHTING_DENOISING_SIZE (65)
#define INDIRECT_LIGHTING_DENOISING_START_END ((INDIRECT_LIGHTING_DENOISING_SIZE - 1) * 0.5f)
#define INDIRECT_LIGHTING_DENOISING_FIREFLY_CUTOFF (3.7f) //0.025f step.

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 normal;
	vec3 hitPosition;
	float hitDistance;
	float roughness;
	float metallic;
	float ambientOcclusion;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 direction;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D indirectLightingTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures3Texture;

//Out parameters.
layout (location = 0) out vec4 indirectLighting;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);

	SceneFeatures features;

	features.normal = UnpackNormal(sceneFeatures2.x);
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.z;
	features.hitDistance = sceneFeatures2.w;
	features.roughness = sceneFeatures3.x;
	features.metallic = sceneFeatures3.y;
	features.ambientOcclusion = sceneFeatures3.z;

	return features;
}

void main()
{
	//Sample the scene features at the current fragment.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Only denoise if there was a hit.
	if (currentFeatures.hitDistance < CATALYST_RAY_TRACING_T_MAXIMUM)
	{
		//Sample neighboring fragments.
		vec3 denoisedIndirectLighting = vec3(0.0f);
		float indirectLightingWeightSum = 0.0f;

		for (float x = -INDIRECT_LIGHTING_DENOISING_START_END; x <= INDIRECT_LIGHTING_DENOISING_START_END; ++x)
		{
			vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

			vec3 sampleIndirectLighting = texture(indirectLightingTexture, sampleCoordinate).rgb;
			SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. How closely aligned are the hit positions to each other?
			*	2. How closely aligned are the normals to each other?
			*	3. Is the average of the fragment below a set threshold? (Fireflies elimination)
			*/
			float sampleWeight = 1.0f;

			sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);
			sampleWeight *= max(dot(currentFeatures.normal, sampleFeatures.normal), 0.0f);
			sampleWeight *= float(CalculateAverage(sampleIndirectLighting) <= INDIRECT_LIGHTING_DENOISING_FIREFLY_CUTOFF);

			denoisedIndirectLighting += sampleIndirectLighting * sampleWeight;
			indirectLightingWeightSum += sampleWeight;
		}
			
		//Normalize the denoised indirect lighting.
		denoisedIndirectLighting *= 1.0f / indirectLightingWeightSum;

		//Write the fragment.
		indirectLighting = vec4(denoisedIndirectLighting, 1.0f);
	}

	else
	{
		//Write the fragment.
		indirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate);
	}
}