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
#define DENOISING_SIZE (69.0f)
#define DENOISING_START_END ((DENOISING_SIZE - 1.0f) * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
	vec3 hitPosition;
	float hitDistance;
	float roughness;
	float metallic;
	float ambientOcclusion;
	float emissive;
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
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 4) uniform sampler2D temporalAccumulationDescriptionBufferTexture;

//Out parameters.
layout (location = 0) out vec4 scene;

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
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, coordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.normal = UnpackNormal(sceneFeatures2.y);
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.z;
	features.hitDistance = sceneFeatures2.w;
	features.roughness = sceneFeatures3.x;
	features.metallic = sceneFeatures3.y;
	features.ambientOcclusion = sceneFeatures3.z;
	features.emissive = sceneFeatures3.w;

	return features;
}

void main()
{
	//Calculate the denoising weight. Denoise less the more accumulations that the temporal accumulation pass has done.
	vec4 temporalAccumulationDescriptionBufferTextureSampler = texture(temporalAccumulationDescriptionBufferTexture, fragmentTextureCoordinate);
	float denoisingWeight = pow(max(1.0f - ((temporalAccumulationDescriptionBufferTextureSampler.z * temporalAccumulationDescriptionBufferTextureSampler.y) / 2048.0f), 0.0f), 256.0f);

	if (denoisingWeight > 0.0f)
	{
		//Sample the scene features at the current fragment.
		vec3 currentScene = texture(sceneTexture, fragmentTextureCoordinate).rgb;
		SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

		//Calculate the start/end.
		float startAndEnd = DENOISING_START_END * denoisingWeight;

		//Sample neighboring fragments.
		vec3 denoisedScene = vec3(0.0f);
		float sceneWeightSum = 0.0f;

		for (float x = -startAndEnd; x <= startAndEnd; ++x)
		{
			vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

			vec3 sampleScene = texture(sceneTexture, sampleCoordinate).rgb;
			SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. How closely aligned are the luminance of the albedos to each other?
			*	2. How closely aligned are the normals to each other?
			*	3. How closely aligned are the hit positions to each other?
			*	4. How closely aligned are the roughness terms to each other?
			*	5. How closely aligned are the metallic terms to each other?
			*	6. How closely aligned are the ambient occlusion terms to each other?
			*	7. How closely aligned are the ambient occlusion terms to each other?
			*	8. Is the sample coordinate valid?
			*/
			float sampleWeight = 1.0f;

			//sampleWeight *= 1.0f - min(CalculateAverage(currentFeatures.albedo) - CalculateAverage(sampleFeatures.albedo), 1.0f);
			sampleWeight *= max(dot(currentFeatures.normal, sampleFeatures.normal), 0.0f);
			sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);
			sampleWeight *= 1.0f - min(abs(currentFeatures.roughness - sampleFeatures.roughness), 1.0f);
			sampleWeight *= 1.0f - min(abs(currentFeatures.metallic - sampleFeatures.metallic), 1.0f);
			sampleWeight *= 1.0f - min(abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion), 1.0f);
			sampleWeight *= 1.0f - min(abs(currentFeatures.emissive - sampleFeatures.emissive), 1.0f);
			sampleWeight *= float(ValidCoordinate(sampleCoordinate));

			denoisedScene += sampleScene * sampleWeight;
			sceneWeightSum += sampleWeight;
		}
				
		//Normalize the denoised scene.
		denoisedScene = sceneWeightSum == 0.0f ? currentScene : denoisedScene / sceneWeightSum;

		//Write the fragment.
		scene = vec4(denoisedScene, 1.0f);
	}

	else
	{
		//Write the fragment.
		scene = vec4(texture(sceneTexture, fragmentTextureCoordinate).rgb, 1.0f);
	}
}