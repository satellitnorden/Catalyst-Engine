//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystTemporalAccumulationCore.glsl"

//Constants.
#define DENOISING_MAXIMUM_TEMPORAL_ACCUMULATIONS (64)
#define INDIRECT_LIGHTING_DENOISING_SIZE (15)
#define INDIRECT_LIGHTING_DENOISING_START_END ((INDIRECT_LIGHTING_DENOISING_SIZE - 1) * 0.5f)
#define DIRECT_LIGHTING_DENOISING_SIZE (3)
#define DIRECT_LIGHTING_DENOISING_START_END ((DIRECT_LIGHTING_DENOISING_SIZE - 1) * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 normal;
	vec3 hitPosition;
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
    layout (offset = 8) bool enabled;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D indirectLightingTexture;
layout (set = 1, binding = 1) uniform sampler2D directLightingTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 4) uniform sampler2D temporalAccumulationBufferTexture;

//Out parameters.
layout (location = 0) out vec4 indirectLighting;
layout (location = 1) out vec4 directLighting;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, coordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.normal = sceneFeatures1.xyz;
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures1.w;
	features.roughness = sceneFeatures2.x;
	features.metallic = sceneFeatures2.y;
	features.ambientOcclusion = sceneFeatures2.z;

	return features;
}

void main()
{
	if (enabled)
	{
		//Sample the temporal accumulation buffer. Calculate a denoising weight depending on how many accumulations have been made.
		AccumulationDescription temporalAccumulationDescription = UnpackAccumulationDescription(floatBitsToUint(texture(temporalAccumulationBufferTexture, fragmentTextureCoordinate).w));
		float temporalAccumulationWeight = 1.0f - float(temporalAccumulationDescription.accumulations / DENOISING_MAXIMUM_TEMPORAL_ACCUMULATIONS);

		//Sample the indirect lighting and scene features at the current fragment.
		vec3 currentIndirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate).rgb;
		SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

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
			*	2. How closely aligned are the roughness terms?
			*	3. How closely aligned are the metallic terms?
			*	4. How closely aligned are the ambient occlusion terms?
			*/
			float sampleWeight = 1.0f;

			sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);
			sampleWeight *= 1.0f - abs(currentFeatures.roughness - sampleFeatures.roughness);
			sampleWeight *= 1.0f - abs(currentFeatures.metallic - sampleFeatures.metallic);
			sampleWeight *= 1.0f - abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion);

			denoisedIndirectLighting += sampleIndirectLighting * sampleWeight;
			indirectLightingWeightSum += sampleWeight;
		}
		
		//Normalize the denoised indirect lighting.
		denoisedIndirectLighting *= 1.0f / indirectLightingWeightSum;

		//Linearly interpolate the denoised indirect lighting with the current indirect lighting based on the temporal accumulation weight.
		denoisedIndirectLighting = mix(currentIndirectLighting, denoisedIndirectLighting, temporalAccumulationWeight);

		//Sample the direct lighting and scene features at the current fragment.
		vec3 currentDirectLighting = texture(directLightingTexture, fragmentTextureCoordinate).rgb;

		//Calculate the denoise direct lighting.
		vec3 denoisedDirectLighting = vec3(0.0f);
		float directLightingWeightSum = 0.0f;

		for (float x = -DIRECT_LIGHTING_DENOISING_START_END; x <= DIRECT_LIGHTING_DENOISING_START_END; ++x)
		{
			vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

			vec3 sampleDirectLighting = texture(directLightingTexture, sampleCoordinate).rgb;
			SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. How closely aligned are the normals?
			*	2. How closely aligned are the hit positions to each other?
			*/
			float sampleWeight = 1.0f;

			sampleWeight *= max(dot(currentFeatures.normal, sampleFeatures.normal), 1.0f);
			sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);

			denoisedDirectLighting += sampleDirectLighting * sampleWeight;
			directLightingWeightSum += sampleWeight;
		}
		
		//Normalize the denoised indirect lighting.
		denoisedDirectLighting *= 1.0f / directLightingWeightSum;

		//Linearly interpolate the denoised indirect lighting with the current indirect lighting based on the temporal accumulation weight.
		denoisedDirectLighting = mix(currentDirectLighting, denoisedDirectLighting, temporalAccumulationWeight);

		//Write the fragment.
		indirectLighting = vec4(denoisedIndirectLighting, 1.0f);
		directLighting = vec4(denoisedDirectLighting, 1.0f);
	}

	else
	{
		//Write the fragment.
		indirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate);
		directLighting = texture(directLightingTexture, fragmentTextureCoordinate);
	}
}