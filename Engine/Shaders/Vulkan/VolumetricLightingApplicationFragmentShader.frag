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
	vec3 hit_position;
	float hit_distance;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D volumetric_lighting_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(scene_features_2_texture, coordinate);

	SceneFeatures features;

	features.hit_position = CalculateWorldPosition(fragment_texture_coordinate, sceneFeatures2.w);
	features.hit_distance = length(features.hit_position - PERCEIVER_WORLD_POSITION);

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample the current volumetric lighting.
	vec3 current_volumetric_lighting = Upsample(volumetric_lighting_texture, fragment_texture_coordinate).rgb;

	//Calculate the volumetric lighting weight.
	float volumetric_lighting_weight = 1.0f - pow(1.0f - min(current_features.hit_distance / VIEW_DISTANCE, 1.0f), volumetricLightingIntensity);
	volumetric_lighting_weight *= max(SmoothStep(1.0f - clamp(current_features.hit_position.y / 1000.0f, 0.0f, 1.0f)), 0.1f);

	//Write the fragment.
	fragment = vec4(current_volumetric_lighting, volumetric_lighting_weight);
}