//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
	vec3 hitPosition;
	float roughness;
	float metallic;
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
layout (set = 1, binding = 4) uniform sampler2D diffuseIrradianceTexture;
layout (set = 1, binding = 5) uniform sampler2D specularIrradianceTexture;
layout (set = 1, binding = 6) uniform sampler2D directLightingTexture;
layout (set = 1, binding = 7) uniform sampler2D volumetricLightingTexture;

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

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.normal = sceneFeatures3.xyz;
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.w;
	features.roughness = sceneFeatures4.x;
	features.metallic = sceneFeatures4.y;
	features.ambientOcclusion = sceneFeatures4.z;

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample the current diffuse irradiance lighting.
	vec3 currentDiffuseIrradiance = texture(diffuseIrradianceTexture, fragmentTextureCoordinate).rgb;

	//Sample the current specular irradiance lighting.
	vec3 currentSpecularIrradiance = mix(currentDiffuseIrradiance, texture(specularIrradianceTexture, fragmentTextureCoordinate).rgb, pow(1.0f - CalculateDiffuseComponent(currentFeatures.roughness, currentFeatures.metallic), 4.0f));

	//Sample the current direct lighting.
	vec3 currentDirectLighting = texture(directLightingTexture, fragmentTextureCoordinate).rgb;

	//Sample the current volumetric lighting.
	vec3 currentVolumetricLighting = texture(volumetricLightingTexture, fragmentTextureCoordinate).rgb;

	//Calculate the indirect lighting.
	vec3 indirectLighting = CalculateIndirectLighting(	normalize(currentFeatures.hitPosition - perceiverWorldPosition),
														currentFeatures.albedo,
														currentFeatures.normal,
														currentFeatures.roughness,
														currentFeatures.metallic,
														currentFeatures.ambientOcclusion,
														currentDiffuseIrradiance,
														currentSpecularIrradiance);

	//Write the fragment.
	scene = vec4(indirectLighting + currentDirectLighting + currentVolumetricLighting, 1.0f);
}