//Includes.
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

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

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float volumetric_lighting_distance;
	layout (offset = 4) float volumetric_lighting_height;
	layout (offset = 8) float volumetric_lighting_thickness;
};

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

	SceneFeatures features;

	if (sceneFeatures2.w == 0.0f)
	{
		features.hit_position = CalculateRayDirection(fragment_texture_coordinate) * volumetric_lighting_distance;
		features.hit_distance = volumetric_lighting_distance;
	}

	else
	{
		features.hit_position = CalculateWorldPosition(fragment_texture_coordinate, sceneFeatures2.w);
		features.hit_distance = length(features.hit_position - PERCEIVER_WORLD_POSITION);
	}

	return features;
}

void CatalystShaderMain()
{
	//Sample the current features.
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample the current volumetric lighting.
	vec3 current_volumetric_lighting = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).rgb;

	//Calculate the volumetric lighting opacity.
	float volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(current_features.hit_distance, volumetric_lighting_distance, current_features.hit_position.y, volumetric_lighting_height, volumetric_lighting_thickness, PERCEIVER_WORLD_POSITION.y);

	//Write the fragment.
	fragment = vec4(current_volumetric_lighting.rgb, volumetric_lighting_opacity);
}