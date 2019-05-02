//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DENOISING_SIZE (15)
#define DENOISING_START_END ((DENOISING_SIZE - 1) * 0.5f)

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
		//Sample the indirect lighting and scene features at the current fragment.
		vec3 currentIndirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate).rgb;
		SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

		//Sample neighboring fragments.
		vec3 denoisedIndirectLighting = vec3(0.0f);
		float weightSum = 1.0f;

		for (float x = -DENOISING_START_END; x <= DENOISING_START_END; ++x)
		{
			vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

			vec3 sampleIndirectLighting = texture(indirectLightingTexture, sampleCoordinate).rgb;
			SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. How closely aligned are the normals?
			*	2. How closely aligned are the hit positions to each other?
			*	3. How closely aligned are the roughness terms?
			*	4. How closely aligned are the metallic terms?
			*	5. How closely aligned are the ambient occlusion terms?
			*/
			float sampleWeight = 1.0f;

			sampleWeight *= max(dot(currentFeatures.normal, sampleFeatures.normal), 1.0f);
			sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);
			sampleWeight *= 1.0f - abs(currentFeatures.roughness - sampleFeatures.roughness);
			sampleWeight *= 1.0f - abs(currentFeatures.metallic - sampleFeatures.metallic);
			sampleWeight *= 1.0f - abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion);

			denoisedIndirectLighting += sampleIndirectLighting * sampleWeight;
			weightSum += sampleWeight;
		}
		
		//Normalize the denoised indirect lighting.
		denoisedIndirectLighting *= 1.0f / weightSum;

		//Linearly interpolate the denoised indirect lighting with the current indirect lighting based on how diffuse the material are.
		denoisedIndirectLighting = mix(currentIndirectLighting, denoisedIndirectLighting, GetSpecularComponent(currentFeatures.roughness, currentFeatures.metallic));

		//Write the fragment.
		indirectLighting = vec4(denoisedIndirectLighting, 1.0f);
		directLighting = texture(directLightingTexture, fragmentTextureCoordinate);
	}

	else
	{
		//Write the fragment.
		indirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate);
		directLighting = texture(directLightingTexture, fragmentTextureCoordinate);
	}
}