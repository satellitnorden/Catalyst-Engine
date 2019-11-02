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
	int materialProperties;
	float luminance;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures4Texture;

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

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.normal = UnpackNormal(sceneFeatures2.y);
	features.materialProperties = floatBitsToInt(sceneFeatures1.w);
	features.luminance = sceneFeatures4.w * sceneFeatures2.z;

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Calculate the luminance lighting.
	float highlightWeight = max(CalculateHighlightWeight(CalculateRayDirection(fragmentTextureCoordinate), currentFeatures.normal, currentFeatures.materialProperties), 0.0f);

	currentFeatures.luminance = mix(currentFeatures.luminance, currentFeatures.luminance + 1.0f, highlightWeight);

	vec3 luminanceLighting = currentFeatures.albedo * currentFeatures.luminance;

	//Write the fragment.
	scene = vec4(luminanceLighting, 1.0f);
}