//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DEPTH_OF_FIELD_BOKEH_BLUR_SAMPLES (29)

vec2 SAMPLE_POINTS[29] = vec2[]
(
	vec2(-1.0f, 0.0f),
	vec2(-0.666667f, -0.666667f),
	vec2(-0.666667f, -0.333333f),
	vec2(-0.666667f, 0.0f),
	vec2(-0.666667f, 0.333333f),
	vec2(-0.666667f, 0.666667f),
	vec2(-0.333333f, -0.666667f),
	vec2(-0.333333f, -0.333333f),
	vec2(-0.333333f, 0.0f),
	vec2(-0.333333f, 0.333333f),
	vec2(-0.333333f, 0.666667f),
	vec2(0.0f, -1.0f),
	vec2(0.0f, -0.666667f),
	vec2(0.0f, -0.333333f),
	vec2(0.0f, 0.0f),
	vec2(0.0f, 0.333333f),
	vec2(0.0f, 0.666667f),
	vec2(0.0f, 1.0f),
	vec2(0.333333f, -0.666667f),
	vec2(0.333333f, -0.333333f),
	vec2(0.333333f, 0.0f),
	vec2(0.333333f, 0.333333f),
	vec2(0.333333f, 0.666667f),
	vec2(0.666667f, -0.666667f),
	vec2(0.666667f, -0.333333f),
	vec2(0.666667f, 0.0f),
	vec2(0.666667f, 0.333333f),
	vec2(0.666667f, 0.666667f),
	vec2(1.0f, 0.0f)
);

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float DEPTH_OF_FIELD_FOCUS_DISTANCE;
	layout (offset = 4) float DEPTH_OF_FIELD_SIZE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Calculate the current view distance.
	float current_view_distance = -(CalculateViewSpacePosition(fragment_texture_coordinate, texture(scene_features_2_texture, fragment_texture_coordinate).w).z);

	//Calculate the depth of field weight for this pixel.
	float depth_of_field_weight = min(current_view_distance / DEPTH_OF_FIELD_FOCUS_DISTANCE, 1.0f);

	//Perform all the samples.
	vec3 blurred_scene = vec3(0.0f);
	float total_weight = 0.0f;

	for (uint i = 0; i < DEPTH_OF_FIELD_BOKEH_BLUR_SAMPLES; ++i)
	{
		//Calculate the unit square coordinates.
		vec2 unit_disk_coordinate = SAMPLE_POINTS[i];

		//Scale the unit disk coordinate depending on the depth of field weight/size.
		unit_disk_coordinate *= depth_of_field_weight;
		unit_disk_coordinate *= DEPTH_OF_FIELD_SIZE;

		//Scale the unit disk coordinate depending on the depth of field weight/size.
		unit_disk_coordinate.y *= ASPECT_RATIO;

		//Calculate the sample coordinate.
		vec2 sample_coordinate = fragment_texture_coordinate + unit_disk_coordinate;

		/*
		*	Calculate the sample weight, depending on these criteria;
		*	
		*	1. Is the sample coordinate a valid coordinate?
		*/
		float sample_weight = 1.0f;

		sample_weight *= float(ValidCoordinate(sample_coordinate));

		blurred_scene += texture(scene_texture, sample_coordinate).rgb * sample_weight;
		total_weight += sample_weight;
	}

	//Normalize the blurred scene.
	blurred_scene = total_weight != 0.0f ? blurred_scene / total_weight : vec3(0.0f);

	//Write the fragment.
	fragment = vec4(blurred_scene, 1.0f);
}