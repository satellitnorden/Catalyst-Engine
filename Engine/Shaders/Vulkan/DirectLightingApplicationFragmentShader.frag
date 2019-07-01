//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
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

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int lightIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures4Texture;
layout (set = 1, binding = 4) uniform sampler2D visibilityTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

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

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample the visibility.
	float visibility = Upsample(visibilityTexture, fragmentTextureCoordinate).x;

	//Calculate the ray direction.
	vec3 rayDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Unpack the relevant light.
	Light light = UnpackLight(lightIndex);

	//Calculate the attenuation.
	float lengthToLight = length(light.position - currentFeatures.hitPosition);
	vec3 lightDirection = vec3(light.position - currentFeatures.hitPosition) / lengthToLight;

	float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

	//Calculate the direct lighting.
	vec3 directLighting = CalculateDirectLight(	-rayDirection,
												lightDirection,
												currentFeatures.albedo,
												currentFeatures.normal,
												currentFeatures.roughness,
												currentFeatures.metallic,
												light.color * light.strength) * attenuation * visibility;

	//Write the fragment.
	fragment = vec4(directLighting, 1.0f);
}