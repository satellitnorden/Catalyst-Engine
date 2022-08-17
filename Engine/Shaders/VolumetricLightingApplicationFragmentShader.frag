//Includes.
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Constants.
#define DEPTH_WEIGHT (4.0f)

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
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_2_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D VOLUMETRIC_LIGHTING_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(SCENE_FEATURES_2_TEXTURE, coordinate);

	SceneFeatures features;

	if (sceneFeatures2.w == 0.0f)
	{
		features.hit_position = CalculateRayDirection(fragment_texture_coordinate) * VOLUMETRIC_LIGHTING_DISTANCE;
		features.hit_distance = VOLUMETRIC_LIGHTING_DISTANCE;
	}

	else
	{
		features.hit_position = CalculateWorldPosition(fragment_texture_coordinate, sceneFeatures2.w);
		features.hit_distance = length(features.hit_position - PERCEIVER_WORLD_POSITION);
	}

	return features;
}

/*
*	Samples the volumetric lighting at the given coordinate.
*/
vec3 SampleVolumetricLighting(vec2 coordinate)
{
	//Sample the current depth.
	float current_depth = LinearizeDepth(texture(SCENE_FEATURES_2_TEXTURE, coordinate).w);

	//Sample the four neighbor samples along with their depth.
	vec3 sample_1_color = texture(VOLUMETRIC_LIGHTING_TEXTURE, coordinate + vec2(0.0f, 0.0f)).rgb;
	float sample_1_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f)).w);
	vec3 sample_2_color = texture(VOLUMETRIC_LIGHTING_TEXTURE, coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).rgb;
	float sample_2_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).w);
	vec3 sample_3_color = texture(VOLUMETRIC_LIGHTING_TEXTURE, coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).rgb;
	float sample_3_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).w);
	vec3 sample_4_color = texture(VOLUMETRIC_LIGHTING_TEXTURE, coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).rgb;
	float sample_4_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).w);

	//Calculate the horizontal and vertical weights.
	float horizontal_weight = fract(coordinate.x * (SCALED_RESOLUTION.x * 0.5f));
	float vertical_weight = fract(coordinate.y * (SCALED_RESOLUTION.y * 0.5f));

	//Calculate all the weights.
	float first_weight = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
	float second_weight = (1.0f - horizontal_weight) * vertical_weight;
	float third_weight = horizontal_weight * (1.0f - vertical_weight);
	float fourth_weight = horizontal_weight * vertical_weight;

	//Alter the weights depending on how closely aligned they are with the current depth.
	first_weight *= pow(1.0f - abs(current_depth - sample_1_depth), DEPTH_WEIGHT);
	second_weight *= pow(1.0f - abs(current_depth - sample_2_depth), DEPTH_WEIGHT);
	third_weight *= pow(1.0f - abs(current_depth - sample_3_depth), DEPTH_WEIGHT);
	fourth_weight *= pow(1.0f - abs(current_depth - sample_4_depth), DEPTH_WEIGHT);

	//Renormalize the weights.
	float total_weight_reciprocal = 1.0f / (first_weight + second_weight + third_weight + fourth_weight);

	first_weight *= total_weight_reciprocal;
	second_weight *= total_weight_reciprocal;
	third_weight *= total_weight_reciprocal;
	fourth_weight *= total_weight_reciprocal;

	//Perform the final blend.
	vec3 final_blend = 	sample_1_color * first_weight
						+ sample_2_color * second_weight
						+ sample_3_color * third_weight
						+ sample_4_color * fourth_weight;

	//Return the final blend.
	return final_blend;
}

void CatalystShaderMain()
{
	//Sample the current features.
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample the current volumetric lighting.
	vec3 current_volumetric_lighting = SampleVolumetricLighting(fragment_texture_coordinate);

	//Calculate the volumetric lighting opacity.
	float volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(current_features.hit_distance, VOLUMETRIC_LIGHTING_DISTANCE, current_features.hit_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, PERCEIVER_WORLD_POSITION.y);

	//Write the fragment.
	fragment = vec4(current_volumetric_lighting.rgb, volumetric_lighting_opacity);
}