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
	layout (offset = 8) int STRIDE;
	layout (offset = 12) int DIRECTION;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SOURCE_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_2_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D SCENE_FEATURES_3_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 scene_feature_2 = texture(SCENE_FEATURES_2_TEXTURE, coordinate);

	SceneFeatures features;

	features.normal = scene_feature_2.xyz;
	features.view_distance = CalculateViewSpacePosition(coordinate, scene_feature_2.w).z;
	return features;
}

void CatalystShaderMain()
{
	//Sample the indirect lighting features at the current fragment.
	vec3 current_indirect_lighting = texture(SOURCE_TEXTURE, fragment_texture_coordinate).rgb;
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample neighboring fragments.
	vec3 denoised_indirect_lighting = vec3(0.0f);
	float total_weight = 0.0f;

	//Calculate the altered stride.
	vec4 scene_features_3 = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate);
	float altered_stride = float(STRIDE) * scene_features_3.x * (1.0f - scene_features_3.y);

	//Remember the minimum to combat fireflies.
	vec3 minimum = vec3(1.0f);

	for (int sample_position = -STRIDE; sample_position <= STRIDE; sample_position += STRIDE)
	{
		vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(sample_position) * float(DIRECTION == 0), float(sample_position) * float(DIRECTION == 1)) * INVERSE_RESOLUTION;

		vec4 sample_indirect_lighting = texture(SOURCE_TEXTURE, sample_coordinate);
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

		denoised_indirect_lighting += sample_indirect_lighting.rgb * sample_weight;
		total_weight += sample_weight;

		minimum = min(minimum, sample_indirect_lighting.rgb * sample_weight);
	}

	//Normalize the denoised indirect lighting.
	denoised_indirect_lighting = total_weight == 0.0f ? current_indirect_lighting : denoised_indirect_lighting / total_weight;

	//Reduce fireflies.
	denoised_indirect_lighting = min(denoised_indirect_lighting, minimum + vec3(1.0f));

	//Write the fragment.
	fragment = vec4(denoised_indirect_lighting, min(total_weight, 1.0f));
}