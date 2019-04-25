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
#define INDIRECT_LIGHTING_DENOISE_SIZE (9)

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

		//Determine the weights for all samples.
		float sampleWeights[INDIRECT_LIGHTING_DENOISE_SIZE];
		sampleWeights[0] = 1.0f;
		vec3 indirectLightingSamples[INDIRECT_LIGHTING_DENOISE_SIZE];
		indirectLightingSamples[0] = currentIndirectLighting;
		vec2 sampleCoordinates[INDIRECT_LIGHTING_DENOISE_SIZE] = vec2[]
		(
			vec2(0.0f),
			fragmentTextureCoordinate + vec2(-INVERSE_WIDTH, 0.0f),
			fragmentTextureCoordinate + vec2(INVERSE_WIDTH, 0.0f),
			fragmentTextureCoordinate + vec2(0.0f, -INVERSE_HEIGHT),
			fragmentTextureCoordinate + vec2(0.0f, -INVERSE_HEIGHT),

			fragmentTextureCoordinate + vec2(-INVERSE_WIDTH, -INVERSE_HEIGHT),
			fragmentTextureCoordinate + vec2(-INVERSE_WIDTH, INVERSE_HEIGHT),
			fragmentTextureCoordinate + vec2(INVERSE_WIDTH, -INVERSE_HEIGHT),
			fragmentTextureCoordinate + vec2(INVERSE_WIDTH, INVERSE_HEIGHT)
		);

		//Calculate the weight sum.
		float weightSum = 1.0f;

		for (int i = 1; i < INDIRECT_LIGHTING_DENOISE_SIZE; ++i)
		{
			indirectLightingSamples[i] = texture(indirectLightingTexture, sampleCoordinates[i]).rgb;
			SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinates[i]);

			/*
			*	Calculate the sample weight based on certein criteria;
			*	
			*	1. How much do the normals align? This weight is less important the more diffuse the material are.
			*	2. How close are the hit position to each other?
			*	3. How closely aligned are the ambient occlusion terms?
			*/
			sampleWeights[i] = 1.0f;

			sampleWeights[i] *= mix(max(dot(currentFeatures.normal, sampleFeatures.normal), 0.0f), 1.0f, GetSpecularComponent(currentFeatures.roughness, currentFeatures.metallic));
			//sampleWeights[i] *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);
			sampleWeights[i] *= 1.0f - abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion);

			weightSum += sampleWeights[i];
		}

		//Add together all the samples.
		float normalizedWeightSum = 1.0f / weightSum;

		vec3 denoisedIndirectLighting = vec3(0.0f);

		for (int i = 0; i < INDIRECT_LIGHTING_DENOISE_SIZE; ++i)
		{
			denoisedIndirectLighting += indirectLightingSamples[i] * (sampleWeights[i] * normalizedWeightSum);
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