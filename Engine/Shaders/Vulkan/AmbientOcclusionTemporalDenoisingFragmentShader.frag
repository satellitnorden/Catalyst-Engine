//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_FEEDBACK_FACTOR (0.99f)
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE (3.0f)
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END ((AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_4_texture;
layout (set = 1, binding = 1) uniform sampler2D previous_ambient_occlusion_texture;
layout (set = 1, binding = 2) uniform sampler2D current_ambient_occlusion_texture;

//Out parameters.
layout (location = 0) out vec4 current_ambient_occlusion;
layout (location = 1) out vec4 ambient_occlusion;

/*
*	Calculates the neighborhood sample weight
*/
float CalculateNeighborhoodSampleWeight(float minimum, float maximum, float previous)
{
	//Calculate the weight.
	float weight = 1.0f;

	weight *= 1.0f - max(minimum - previous, 0.0f);
	weight *= 1.0f - max(previous - maximum, 0.0f);

	//Bias the weight.
	weight = weight * weight;

	//Return the weight.
	return weight;
}

void main()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - currentFrameJitter;

	//Sample the current ambient occlusion texture.
	vec4 current_ambient_occlusion_texture_sampler = texture(current_ambient_occlusion_texture, unjittered_screen_coordinate);

	//Calculate the minimum/maximum ambient occlusion values in the neighborhood of the current frame.
	float minimum = current_ambient_occlusion_texture_sampler.x;
	float maximum = current_ambient_occlusion_texture_sampler.x;

	for (float x = -AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; x <= AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++x)
	{
		for (float y = -AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; y <= AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++y)
		{
			vec2 sample_coordinate = unjittered_screen_coordinate + vec2(x, y) * inverseScaledResolution;
		
			float neighbordhood_sample = texture(current_ambient_occlusion_texture, sample_coordinate).x;

			minimum = min(minimum, neighbordhood_sample);
			maximum = max(maximum, neighbordhood_sample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(scene_features_4_texture, unjittered_screen_coordinate).xy;

	//Sample the previous ambient occlusion texture.
	vec4 previous_ambient_occlusion_texture_sampler = texture(previous_ambient_occlusion_texture, previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How far off is the previous ambient occlusion from the minimum/maximum in the neighborhood?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= CalculateNeighborhoodSampleWeight(minimum, maximum, previous_ambient_occlusion_texture_sampler.x);


	//Blend the previous and the current ambient occlusion.
	float blended_ambient_occlusion = mix(current_ambient_occlusion_texture_sampler.x, previous_ambient_occlusion_texture_sampler.x, AMBIENT_OCCLUSION_TEMPORAL_DENOISING_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	current_ambient_occlusion = vec4(vec3(blended_ambient_occlusion), 1.0f);
	ambient_occlusion = vec4(vec3(blended_ambient_occlusion), 1.0f);
}