//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	float hitDistance;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D volumetricLightingTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.hitDistance = sceneFeatures2.w;

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample the current volumetric lighting.
	vec3 currentVolumetricLighting = texture(volumetricLightingTexture, fragmentTextureCoordinate).rgb;

	//Calculate the volumetric lighting weight.
	float volumetricLightingWeight = 1.0f - pow(1.0f - min(currentFeatures.hitDistance / viewDistance, 1.0f), volumetricLightingIntensity);

	//Write the fragment.
	fragment = vec4(currentVolumetricLighting, volumetricLightingWeight);
}