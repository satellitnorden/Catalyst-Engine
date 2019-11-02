//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
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
	vec3 view_direction;
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
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures4Texture;
layout (set = 1, binding = 3) uniform sampler2D ambientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, coordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);
	vec4 sceneFeatures4 = texture(sceneFeatures4Texture, coordinate);
	vec4 ambientOcclusion = ambientOcclusionMode == AMBIENT_OCCLUSION_MODE_NONE ? vec4(1.0f) : Upsample(ambientOcclusionTexture, coordinate);

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.normal = UnpackNormal(sceneFeatures2.y);
	features.view_direction = CalculateRayDirection(coordinate);
	features.roughness = sceneFeatures4.x;
	features.metallic = sceneFeatures4.y;
	features.luminance = sceneFeatures4.w * sceneFeatures2.z;
	features.ambientOcclusion = pow(sceneFeatures4.z * pow(ambientOcclusion.x, AMBIENT_OCCLUSION_POWER), AMBIENT_OCCLUSION_POWER);

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Calculate the indirect lighting.
	vec3 indirectLighting = CalculateIndirectLighting(	currentFeatures.view_direction,
														currentFeatures.albedo,
														currentFeatures.normal,
														currentFeatures.roughness,
														currentFeatures.metallic,
														currentFeatures.ambientOcclusion,
														SkyColor(currentFeatures.normal),
														vec3(0.0f));

	//Write the fragment.
	scene = vec4(indirectLighting, 1.0f);
}