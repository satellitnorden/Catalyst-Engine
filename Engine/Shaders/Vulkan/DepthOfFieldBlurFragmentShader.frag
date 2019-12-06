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
#define DEPTH_OF_FIELD_BLUR_SIZE (4.0f)
#define DEPTH_OF_FIELD_BLUR_START_END (DEPTH_OF_FIELD_BLUR_SIZE * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	float depth;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 direction;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 scene_features_2 = texture(scene_features_2_texture, coordinate);

	SceneFeatures features;

	features.depth = LinearizeDepth(scene_features_2.w);

	return features;
}

void main()
{
	//Sample the current scene and scene features at the current fragment.
	vec3 current_scene = texture(scene_texture, fragment_texture_coordinate).rgb;
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample neighboring fragments.
	vec3 blurred_scene = vec3(0.0f);
	float blurred_scene_sum = 0.0f;

	for (float x = -DEPTH_OF_FIELD_BLUR_START_END; x <= DEPTH_OF_FIELD_BLUR_START_END; ++x)
	{
		vec2 sample_coordinate = fragment_texture_coordinate + vec2(x, x) * direction;

		vec3 sample_scene = texture(scene_texture, sample_coordinate).rgb;
		SceneFeatures sample_features = SampleSceneFeatures(sample_coordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. Is the sample coordinate valid?
		*	2. Is the depth behind or roughly the same?
		*/
		float sample_weight = 1.0f;

		sample_weight *= float(ValidCoordinate(sample_coordinate));
		sample_weight *= 1.0f - clamp(current_features.depth - sample_features.depth, 0.0f, 1.0f);

		blurred_scene += sample_scene * sample_weight;
		blurred_scene_sum += sample_weight;
	}

	//Normalize the blurred scene.
	blurred_scene = blurred_scene_sum == 0.0f ? current_scene : blurred_scene / blurred_scene_sum;

	//Write the fragment.
	fragment = vec4(blurred_scene, 1.0f);
}