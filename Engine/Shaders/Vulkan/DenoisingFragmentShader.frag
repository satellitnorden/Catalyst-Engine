//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define INVERSE_WIDTH (1.0f / 1920.0f)
#define INVERSE_HEIGHT (1.0f / 1080.0f)
#define INDIRECT_LIGHTING_DENOISE_SIZE (5)
#define INDIRECT_LIGHTING_DENOISE_RANGE ((INDIRECT_LIGHTING_DENOISE_SIZE - 1) / 2)
#define INDIRECT_LIGHTING_CONTRIBUTION (1.0f / (INDIRECT_LIGHTING_DENOISE_SIZE * INDIRECT_LIGHTING_DENOISE_SIZE - 1))

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 normal;
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
	features.hitDistance = sceneFeatures1.w;
	features.roughness = sceneFeatures2.x;
	features.metallic = sceneFeatures2.y;
	features.ambientOcclusion = sceneFeatures2.z;

	return features;
}

void main()
{
	if (enabled)
	{
		//Sample the indirect lighting and scene features.
		vec3 indirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate).rgb;
		SceneFeatures features = SampleSceneFeatures(fragmentTextureCoordinate);

		//Denoise the indirect lighting term.
		vec3 denoisedIndirectLighting = vec3(0.0f);

		for (int i = -INDIRECT_LIGHTING_DENOISE_RANGE; i <= INDIRECT_LIGHTING_DENOISE_RANGE; ++i)
		{
			for (int j = -INDIRECT_LIGHTING_DENOISE_RANGE; j <= INDIRECT_LIGHTING_DENOISE_RANGE; ++j)
			{
				if (i == 0 && j == 0)
				{
					continue;
				}

				vec2 sampleTextureCoordinate = fragmentTextureCoordinate + vec2(INVERSE_WIDTH, INVERSE_HEIGHT) * vec2(i, j);

				vec3 sampleIndirectLighting = texture(indirectLightingTexture, sampleTextureCoordinate).rgb;
				SceneFeatures sampleFeatures = SampleSceneFeatures(sampleTextureCoordinate);

				/*
				*	Blend the indirect lighting and the sample indirect lighting based on a few different criteria;
				*	
				*	1. How much do the normals align? This weight is less important the more diffuse the material are.
				*	2. How close are the hit distances to each other?
				*	3. How closely aligned are the ambient occlusion terms?
				*	4. How far away is the fragment?
				*/
				float weight1 = mix(max(dot(features.normal, sampleFeatures.normal), 0.0f), 1.0f, GetSpecularComponent(features.roughness, features.metallic));
				float weight2 = max(1.0f - abs(features.hitDistance - sampleFeatures.hitDistance), 0.0f);
				float weight3 = 1.0f - abs(features.ambientOcclusion - sampleFeatures.ambientOcclusion);
				float weight4 = 1.0f - clamp(length(vec2(i, j)) / INDIRECT_LIGHTING_DENOISE_RANGE, 0.0f, 1.0f);
				float finalWeight = weight1 * weight2 * weight3 * weight4;

				denoisedIndirectLighting += mix(indirectLighting, sampleIndirectLighting, finalWeight) * INDIRECT_LIGHTING_CONTRIBUTION;
			}
		}

		//Write the fragment.
		if (gl_FragCoord.x < 100.0f && gl_FragCoord.y < 100.0f)
		{
			fragment = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		else
		{
			fragment = vec4(denoisedIndirectLighting, 1.0f)/* + texture(directLightingTexture, fragmentTextureCoordinate)*/;
		}
	}

	else
	{
		//Write the fragment.
		if (gl_FragCoord.x < 100.0f && gl_FragCoord.y < 100.0f)
		{
			fragment = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		else
		{
			fragment = texture(indirectLightingTexture, fragmentTextureCoordinate)/* + texture(directLightingTexture, fragmentTextureCoordinate)*/;
		}
	}
}