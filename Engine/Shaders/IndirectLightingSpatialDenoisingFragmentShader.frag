//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	float view_distance;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 INVERSE_RESOLUTION;
	layout (offset = 8) uint SOURCE_RENDER_TARGET_INDEX;
	layout (offset = 12) int STRIDE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 scene_feature_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

	SceneFeatures features;

	features.view_distance = CalculateViewSpacePosition(coordinate, scene_feature_2.w).z;

	return features;
}

void CatalystShaderMain()
{
	//Sample the indirect lighting features at the current fragment.
	vec4 current_indirect_lighting = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample neighboring fragments.
	vec3 denoised_indirect_lighting = vec3(0.0f);
	float indirect_lighting_weight_sum = 0.0f;

	for (int y = -STRIDE; y <= STRIDE; y += STRIDE)
	{
		for (int x = -STRIDE; x <= STRIDE; x += STRIDE)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(x), float(y)) * INVERSE_RESOLUTION;

			vec4 sample_indirect_lighting = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate);
			SceneFeatures sample_features = SampleSceneFeatures(sample_coordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. Is the sample coordinate valid?
			*	2. How closely aligned are the depths to each other?
			*	3. What is the weight of the indirect lighting sample itself?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));
			sample_weight *= 1.0f - min(abs(current_features.view_distance - sample_features.view_distance), 1.0f);
			sample_weight *= sample_indirect_lighting.a;

			denoised_indirect_lighting += sample_indirect_lighting.rgb * sample_weight;
			indirect_lighting_weight_sum += sample_weight;
		}
	}

	//Normalize the denoised indirect lighting.
	denoised_indirect_lighting = indirect_lighting_weight_sum == 0.0f ? current_indirect_lighting.rgb : denoised_indirect_lighting / indirect_lighting_weight_sum;

	//Write the fragment.
	fragment = vec4(denoised_indirect_lighting, min(indirect_lighting_weight_sum, 1.0f));
}