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
	float hitDistance;
	float roughness;
	float metallic;
	float ambientOcclusion;
	float thickness;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures3Texture;
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
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);
	vec4 ambientOcclusion = ambientOcclusionMode == AMBIENT_OCCLUSION_MODE_NONE ? vec4(1.0f) : Upsample(ambientOcclusionTexture, coordinate);

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.normal = UnpackNormal(sceneFeatures2.x);
	features.view_direction = CalculateRayDirection(coordinate);
	features.roughness = sceneFeatures3.x;
	features.metallic = sceneFeatures3.y;
	features.ambientOcclusion = pow(sceneFeatures3.z * pow(ambientOcclusion.x, AMBIENT_OCCLUSION_POWER), AMBIENT_OCCLUSION_POWER);
	features.thickness = GLOBAL_MATERIALS[int(sceneFeatures1.w * 255.0f)].thickness;

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
														SkyColor(mix(vec3(0.0f, 1.0f, 0.0f), currentFeatures.normal, currentFeatures.thickness)),
														vec3(0.0f));

	//Write the fragment.
	scene = vec4(indirectLighting, 1.0f);
}