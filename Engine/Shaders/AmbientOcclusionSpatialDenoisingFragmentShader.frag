//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DENOISE_SIZE (3)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 _Normal;
	float _ViewDistance;
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
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_2_HALF_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 scene_features_2 = texture(SCENE_FEATURES_2_HALF_TEXTURE, coordinate);

	SceneFeatures features;

	features._Normal = scene_features_2.xyz;
	features._ViewDistance = CalculateViewSpacePosition(coordinate, scene_features_2.w).z;

	return features;
}

void CatalystShaderMain()
{
	//Sample the ambient occlusion features at the current fragment.
	float current_ambient_occlusion = texture(SOURCE_TEXTURE, fragment_texture_coordinate).r;
	SceneFeatures current_scene_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample neighboring fragments.
	float denoised_ambient_occlusion = 0.0f;
	float weight_sum = 0.0f;

	for (int sample_position = -STRIDE; sample_position <= STRIDE; sample_position += STRIDE)
	{
		vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(sample_position) * float(DIRECTION == 0), float(sample_position) * float(DIRECTION == 1)) * INVERSE_RESOLUTION;

		float sample_ambient_occlusion = texture(SOURCE_TEXTURE, sample_coordinate).r;
		SceneFeatures sample_scene_features = SampleSceneFeatures(sample_coordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. Is the sample coordinate valid?
		*	2. How closely aligned are the depths to each other?
		*/
		float sample_weight = 1.0f;

		sample_weight *= float(ValidCoordinate(sample_coordinate));
		sample_weight *= 1.0f - SmoothStep(min(abs(current_scene_features._ViewDistance - sample_scene_features._ViewDistance), 1.0f));

		denoised_ambient_occlusion += sample_ambient_occlusion * sample_weight;
		weight_sum += sample_weight;
	}
	
	//Normalize the denoised ambient occlusion. 
	denoised_ambient_occlusion = weight_sum == 0.0f ? current_ambient_occlusion : denoised_ambient_occlusion / weight_sum;

	//Write the fragment.
	fragment = vec4(vec3(denoised_ambient_occlusion), 1.0f);
}