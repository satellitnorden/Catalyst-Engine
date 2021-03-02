//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 normal;
	float view_distance;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 INVERSE_RESOLUTION;
	layout (offset = 8) uint SOURCE_RENDER_TARGET_INDEX;
	layout (offset = 12) uint _SCENE_FEATURES_2_RENDER_TARGET_INDEX;
	layout (offset = 16) uint _SCENE_FEATURES_3_RENDER_TARGET_INDEX;
	layout (offset = 20) int STRIDE;
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
	vec4 scene_feature_2 = texture(sampler2D(RENDER_TARGETS[_SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

	SceneFeatures features;

	features.normal = scene_feature_2.xyz;
	features.view_distance = CalculateViewSpacePosition(coordinate, scene_feature_2.w).z;
	return features;
}

void CatalystShaderMain()
{
	//Sample the indirect lighting features at the current fragment.
	vec4 current_indirect_lighting = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample neighboring fragments.
	vec4 denoised_indirect_lighting = vec4(0.0f);
	float total_weight = 0.0f;

	//Calculate the altered stride.
	vec4 scene_features_3 = texture(sampler2D(RENDER_TARGETS[_SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);
	float altered_stride = float(STRIDE) * scene_features_3.x * (1.0f - scene_features_3.y);

	//Remember the minimum of all samples, used to reduce fireflies.
	vec3 minimum = current_indirect_lighting.rgb;

	for (int y = -1; y <= 1; y += 1)
	{
		for (int x = -1; x <= 1; x += 1)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(x), float(y)) * altered_stride * INVERSE_RESOLUTION;

			vec4 sample_indirect_lighting = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate);
			SceneFeatures sample_features = SampleSceneFeatures(sample_coordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. Is the sample coordinate valid?
			*	2. How closely aligned are the normals to each other?
			*	3. How closely aligned are the depths to each other?
			*	4. What is the hit percent of the previous sample?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));
			sample_weight *= max(dot(current_features.normal, sample_features.normal), 0.0f);
			sample_weight *= 1.0f - min(abs(current_features.view_distance - sample_features.view_distance), 1.0f);
			sample_weight *= sample_indirect_lighting.a;

			denoised_indirect_lighting += sample_indirect_lighting * sample_weight;
			total_weight += sample_weight;

			minimum = mix(minimum, min(minimum, sample_indirect_lighting.rgb), sample_weight);
		}
	}

	//Normalize the denoised indirect lighting.
	denoised_indirect_lighting = total_weight == 0.0f ? current_indirect_lighting : denoised_indirect_lighting / total_weight;

	//Reduce fireflies. (:
	denoised_indirect_lighting.rgb = mix(denoised_indirect_lighting.rgb, minimum, min(LengthSquared3(minimum - denoised_indirect_lighting.rgb), 1.0f));

	//Write the fragment.
	fragment = vec4(denoised_indirect_lighting.rgb, float(denoised_indirect_lighting.a > 0.0f));
}