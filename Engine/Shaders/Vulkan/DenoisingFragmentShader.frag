//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DENOISING_SIZE (16.0f)
#define DENOISING_START_END (DENOISING_SIZE * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
	vec3 hitPosition;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 direction;
	layout (offset = 8) float stride;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D diffuseIrradianceTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 diffuseIrradiance;

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

	SceneFeatures features;

	features.normal = sceneFeatures2.xyz;
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.w;

	return features;
}

void main()
{
	//Sample the diffuse irradiance features at the current fragment.
	vec3 currentDiffuseIrradiance = texture(diffuseIrradianceTexture, fragmentTextureCoordinate).rgb;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Calculate the start/end.
	float startAndEnd = DENOISING_START_END * stride;

	//Sample neighboring fragments.
	vec3 denoisedDiffuseIrradiance = vec3(0.0f);
	float diffuseIrradianceWeightSum = 0.0f;

	for (float x = -startAndEnd; x <= startAndEnd; x += stride)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

		vec3 sampleDiffuseIrradiance = texture(diffuseIrradianceTexture, sampleCoordinate).rgb;
		SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. How closely aligned are the normals to each other?
		*	2. How closely aligned are the hit positions to each other?
		*	3. Is the sample coordinate valid?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= pow(max(dot(currentFeatures.normal, sampleFeatures.normal), 0.0f), 1.0f);
		sampleWeight *= pow(1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f), 1.0f);
		sampleWeight *= float(ValidCoordinate(sampleCoordinate));

		denoisedDiffuseIrradiance += sampleDiffuseIrradiance * sampleWeight;
		diffuseIrradianceWeightSum += sampleWeight;
	}

	//Normalize the denoised diffuse irradiance. 
	denoisedDiffuseIrradiance = diffuseIrradianceWeightSum == 0.0f ? currentDiffuseIrradiance : denoisedDiffuseIrradiance / diffuseIrradianceWeightSum;

	//Write the fragment.
	diffuseIrradiance = vec4(denoisedDiffuseIrradiance, 1.0f);
}