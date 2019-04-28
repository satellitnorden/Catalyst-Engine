//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DENOISING_SIZE (2.0f)
#define INVERSE_WIDTH (1.0f / 1920.0f)
#define INVERSE_HEIGHT (1.0f / 1080.0f)
#define WEIGHT_EXPONENT (1.0f)

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
    layout (offset = 0) bool enabled;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D indirectLightingTexture;
layout (set = 1, binding = 1) uniform sampler2D directLightingTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

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
		vec3 minimumIndirectLighting = currentIndirectLighting;
		vec3 maximumIndirectLighting = currentIndirectLighting;

		vec3 denoisedIndirectLighting = vec3(0.0f);
		float weightSum = 0.0f;

		for (float x = -DENOISING_SIZE; x <= DENOISING_SIZE; ++x)
		{
			for (float y = -DENOISING_SIZE; y <= DENOISING_SIZE; ++y)
			{
				vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, y) * vec2(INVERSE_WIDTH, INVERSE_HEIGHT);

				vec3 sampleIndirectLighting = texture(indirectLightingTexture, sampleCoordinate).rgb;
				SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

				/*
				*	Calculate the sample weight based on certain criteria;
				*	
				*	1. How much do the normals align? This weight is less important the more diffuse the material are.
				*	2. How close are the hit positions to each other? This weight is less important the more diffuse the material are.
				*	3. How closely aligned are the ambient occlusion terms?
				*/
				float specularComponent = GetSpecularComponent(currentFeatures.roughness, currentFeatures.metallic);

				float sampleWeight = 1.0f;

				sampleWeight *= mix(max(dot(currentFeatures.normal, sampleFeatures.normal), 0.0f), 1.0f, specularComponent);
				sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition) * specularComponent, 1.0f);
				sampleWeight *= 1.0f - abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion);

				sampleWeight = pow(sampleWeight, WEIGHT_EXPONENT);

				denoisedIndirectLighting += sampleIndirectLighting * sampleWeight;
				weightSum += sampleWeight;

				minimumIndirectLighting = min(minimumIndirectLighting, sampleIndirectLighting);
				maximumIndirectLighting = max(maximumIndirectLighting, sampleIndirectLighting);
			}
		}
		
		denoisedIndirectLighting *= 1.0f / weightSum;

		//Write the fragment.
		fragment = vec4(denoisedIndirectLighting, 1.0f) + texture(directLightingTexture, fragmentTextureCoordinate);
	}

	else
	{
		//Write the fragment.
		fragment = texture(indirectLightingTexture, fragmentTextureCoordinate) + texture(directLightingTexture, fragmentTextureCoordinate);
	}
}