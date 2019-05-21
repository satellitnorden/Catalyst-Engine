//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	float roughness;
	float metallic;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D indirectLightingTexture;
layout (set = 1, binding = 1) uniform sampler2D directLightingTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures3Texture;

//Out parameters.
layout (location = 0) out vec4 indirectLighting;
layout (location = 1) out vec4 directLighting;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, coordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.roughness = sceneFeatures3.x;
	features.metallic = sceneFeatures3.y;

	return features;
}

void main()
{
	//Sample the current indirect lighting.
	vec3 currentIndirectLighting = texture(indirectLightingTexture, fragmentTextureCoordinate).rgb;

	//Sample the current direct lighting.
	vec3 currentDirectLighting = texture(directLightingTexture, fragmentTextureCoordinate).rgb;

	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Detach the albedo detail from the current direct lighting.
	currentDirectLighting /= mix(vec3(1.0f), currentFeatures.albedo, CalculateDiffuseComponent(currentFeatures.roughness, currentFeatures.metallic));

	//Write the fragments.
	indirectLighting = vec4(currentIndirectLighting, 1.0f);
	directLighting = vec4(currentDirectLighting, 1.0f);
}