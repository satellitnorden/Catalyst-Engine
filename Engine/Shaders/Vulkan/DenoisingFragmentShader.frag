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
#define DENOISING_SIZE (65.0f)
#define DENOISING_START_END ((DENOISING_SIZE - 1.0f) * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 geometryNormal;
	vec3 shadingNormal;
	vec3 hitPosition;
	float hitDistance;
	float roughness;
	float metallic;
	float ambientOcclusion;
	float luminance;
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
layout (set = 1, binding = 1) uniform sampler2D directLightingTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 4) uniform sampler2D temporalAccumulationDescriptionBufferTexture;

//Out parameters.
layout (location = 0) out vec4 indirectLighting;
layout (location = 1) out vec4 directLighting;

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

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);

	SceneFeatures features;

	features.geometryNormal = UnpackNormal(sceneFeatures2.x);
	features.shadingNormal = UnpackNormal(sceneFeatures2.y);
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.z;
	features.hitDistance = sceneFeatures2.w;
	features.roughness = sceneFeatures3.x;
	features.metallic = sceneFeatures3.y;
	features.ambientOcclusion = sceneFeatures3.z;
	features.luminance = sceneFeatures3.w;

	return features;
}

void main()
{
	//Denoise the indirect lighting.
	{
		//Calculate the denoising weight. Denoise less the more accumulations that the temporal accumulation pass has done.
		vec4 temporalAccumulationDescriptionBufferTextureSampler = texture(temporalAccumulationDescriptionBufferTexture, fragmentTextureCoordinate);
		float denoisingWeight = pow(max(1.0f - ((temporalAccumulationDescriptionBufferTextureSampler.z * temporalAccumulationDescriptionBufferTextureSampler.y) / 1024.0f), 0.0f), 2.0f);

		if (denoisingWeight > 0.0f)
		{
			//Sample the indirect lighting features at the current fragment.
			vec3 currentIndirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate).rgb;
			SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

			//Calculate the diffuse component.
			float diffuseComponent = CalculateDiffuseComponent(currentFeatures.roughness, currentFeatures.metallic);

			//Calculate the start/end.
			float startAndEnd = DENOISING_START_END * denoisingWeight * diffuseComponent;

			//Sample neighboring fragments.
			vec3 denoisedIndirectLighting = vec3(0.0f);
			float indirectLightingWeightSum = 0.0f;

			for (float x = -startAndEnd; x <= startAndEnd; ++x)
			{
				vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

				vec3 sampleIndirectLighting = texture(indirectLightingTexture, sampleCoordinate).rgb;
				SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

				/*
				*	Calculate the sample weight based on certain criteria;
				*	
				*	1. How closely aligned are the normals to each other?
				*	2. How closely aligned are the hit positions to each other?
				*	3. Is the sample coordinate valid?
				*/
				float sampleWeight = 1.0f;

				sampleWeight *= max(mix(dot(currentFeatures.shadingNormal, sampleFeatures.shadingNormal), dot(currentFeatures.geometryNormal, sampleFeatures.geometryNormal), diffuseComponent), 0.0f);
				sampleWeight *= pow(1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f), 1.0f);
				sampleWeight *= float(ValidCoordinate(sampleCoordinate));

				denoisedIndirectLighting += sampleIndirectLighting * sampleWeight;
				indirectLightingWeightSum += sampleWeight;
			}

			//Normalize the denoised scene. Blend with the minimum sample based on the variance to fight fireflies.
			denoisedIndirectLighting = indirectLightingWeightSum == 0.0f ? currentIndirectLighting : denoisedIndirectLighting / indirectLightingWeightSum;

			//Write the fragment.
			indirectLighting = vec4(denoisedIndirectLighting, 1.0f);
		}

		else
		{
			//Write the fragment.
			indirectLighting = vec4(texture(indirectLightingTexture, fragmentTextureCoordinate).rgb, 1.0f);
		}
	}

	//Denoise the direct lighting.
	{
		//Calculate the denoising weight. Denoise less the more accumulations that the temporal accumulation pass has done.
		vec4 temporalAccumulationDescriptionBufferTextureSampler = texture(temporalAccumulationDescriptionBufferTexture, fragmentTextureCoordinate);
		float denoisingWeight = pow(max(1.0f - ((temporalAccumulationDescriptionBufferTextureSampler.z * temporalAccumulationDescriptionBufferTextureSampler.y) / 1024.0f), 0.0f), 128.0f);

		if (denoisingWeight > 0.0f)
		{
			//Sample the direct lighting features at the current fragment.
			vec3 currentDirectLighting = texture(directLightingTexture, fragmentTextureCoordinate).rgb;
			SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

			//Calculate the start/end.
			float startAndEnd = DENOISING_START_END * denoisingWeight;

			//Sample neighboring fragments.
			vec3 denoisedDirectLighting = vec3(0.0f);
			float directLightingWeightSum = 0.0f;

			for (float x = -startAndEnd; x <= startAndEnd; ++x)
			{
				vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

				vec3 sampleDirectLighting = texture(directLightingTexture, sampleCoordinate).rgb;
				SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

				/*
				*	Calculate the sample weight based on certain criteria;
				*	
				*	1. How closely aligned are the normals to each other?
				*	2. How closely aligned are the hit positions to each other?
				*	3. How closely aligned are the roughness terms to each other?
				*	4. How closely aligned are the metallic terms to each other?
				*	5. How closely aligned are the ambient occlusion terms to each other?
				*	6. How closely aligned are the luminance terms to each other?
				*	7. Is the sample coordinate valid?
				*/
				float sampleWeight = 1.0f;

				sampleWeight *= max(dot(currentFeatures.shadingNormal, sampleFeatures.shadingNormal), 0.0f);
				sampleWeight *= pow(1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f), 1.0f);
				sampleWeight *= 1.0f - min(abs(currentFeatures.roughness - sampleFeatures.roughness), 1.0f);
				sampleWeight *= 1.0f - min(abs(currentFeatures.metallic - sampleFeatures.metallic), 1.0f);
				sampleWeight *= 1.0f - min(abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion), 1.0f);
				sampleWeight *= 1.0f - min(abs(currentFeatures.luminance - sampleFeatures.luminance), 1.0f);
				sampleWeight *= float(ValidCoordinate(sampleCoordinate));

				denoisedDirectLighting += sampleDirectLighting * sampleWeight;
				directLightingWeightSum += sampleWeight;
			}
						
			//Normalize the denoised scene. Blend with the minimum sample based on the variance to fight fireflies.
			denoisedDirectLighting = directLightingWeightSum == 0.0f ? currentDirectLighting : denoisedDirectLighting / directLightingWeightSum;

			//Write the fragment.
			directLighting = vec4(denoisedDirectLighting, 1.0f);
		}

		else
		{
			//Write the fragment.
			directLighting = vec4(texture(directLightingTexture, fragmentTextureCoordinate).rgb, 1.0f);
		}
	}
}