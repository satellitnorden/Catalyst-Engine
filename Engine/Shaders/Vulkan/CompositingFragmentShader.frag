//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
	vec3 hitPosition;
	int materialProperties;
	float hitDistance;
	float roughness;
	float metallic;
	float luminance;
	float ambientOcclusion;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures4Texture;
layout (set = 1, binding = 4) uniform sampler2D ambientOcclusionTexture;
layout (set = 1, binding = 5) uniform sampler2D diffuseIrradianceTexture;
layout (set = 1, binding = 6) uniform sampler2D directLightingTexture;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, coordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);
	vec4 sceneFeatures4 = texture(sceneFeatures4Texture, coordinate);
	vec4 ambientOcclusion = Upsample(ambientOcclusionTexture, coordinate);

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.normal = sceneFeatures3.xyz;
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.w;
	features.materialProperties = floatBitsToInt(sceneFeatures3.w);
	features.hitDistance = sceneFeatures2.w;
	features.roughness = sceneFeatures4.x;
	features.metallic = sceneFeatures4.y;
	features.luminance = sceneFeatures4.w * sceneFeatures1.w;
	features.ambientOcclusion = pow(sceneFeatures4.z * pow(ambientOcclusion.x, 2.0f), 2.0f);

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample the current diffuse irradiance lighting.
	vec3 currentDiffuseIrradiance;

	if (diffuseIrradianceMode == DIFFUSE_IRRADIANCE_MODE_SIMPLE)
	{
		currentDiffuseIrradiance = vec3(ambientIlluminationIntensity);
	}

	else if (diffuseIrradianceMode == DIFFUSE_IRRADIANCE_MODE_RAY_TRACED)
	{
		currentDiffuseIrradiance = Upsample(diffuseIrradianceTexture, fragmentTextureCoordinate).rgb;
	}

	//Sample the current direct lighting.
	vec3 currentDirectLighting = texture(directLightingTexture, fragmentTextureCoordinate).rgb;

	//Calculate the indirect lighting.
	vec3 indirectLighting = CalculateIndirectLighting(	normalize(currentFeatures.hitPosition - perceiverWorldPosition),
														currentFeatures.albedo,
														currentFeatures.normal,
														currentFeatures.roughness,
														currentFeatures.metallic,
														currentFeatures.ambientOcclusion,
														currentDiffuseIrradiance,
														vec3(0.0f));

	//Write the fragment.
	scene = vec4(indirectLighting + currentDirectLighting, 1.0f);
	//scene = vec4(vec3(currentFeatures.ambientOcclusion), 1.0f);
}