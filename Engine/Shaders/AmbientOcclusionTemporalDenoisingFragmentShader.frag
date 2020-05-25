//Constants.
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR (0.5f)
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_BONUS_FEEDBACK_FACTOR (0.99f - AMBIENT_OCCLUSION_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR)
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE (3.0f)
#define AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END ((AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint PREVIOUS_RENDER_TARGET_INDEX;
	layout (offset = 4) uint CURRENT_RENDER_TARGET_INDEX;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 current_ambient_occlusion;
layout (location = 1) out vec4 ambient_occlusion;

/*
*	Calculates the neighborhood weight.
*/
float NeighborhoodWeight(float previous, float minimum, float maximum)
{
	//Calculate the weight.
	float weight = 1.0f;

	weight *= 1.0f - clamp(minimum - previous, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous - maximum, 0.0f, 1.0f);

	//Bias the weight.
	weight = weight * weight;

	//Return the weight.
	return weight;
}

void CatalystShaderMain()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - currentFrameJitter;

	//Sample the current ambient occlusion texture.
	vec4 current_ambient_occlusion_texture_sampler = texture(sampler2D(RENDER_TARGETS[CURRENT_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), unjittered_screen_coordinate);

	//Calculate the minimum/maximum ambient occlusion values in the neighborhood of the current frame.
	float minimum = current_ambient_occlusion_texture_sampler.x;
	float maximum = current_ambient_occlusion_texture_sampler.x;

	for (float x = -AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; x <= AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++x)
	{
		for (float y = -AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; y <= AMBIENT_OCCLUSION_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++y)
		{
			vec2 sample_coordinate = unjittered_screen_coordinate + vec2(x, y) * INVERSE_SCALED_RESOLUTION * 2.0f;
		
			float neighbordhood_sample = texture(sampler2D(RENDER_TARGETS[CURRENT_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate).x;

			minimum = min(minimum, neighbordhood_sample);
			maximum = max(maximum, neighbordhood_sample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), unjittered_screen_coordinate).xy;

	//Sample the previous ambient occlusion texture.
	vec4 previous_ambient_occlusion_texture_sampler = texture(sampler2D(RENDER_TARGETS[PREVIOUS_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How closely are the previous sample in the neighborhood of the current sample?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= NeighborhoodWeight(previous_ambient_occlusion_texture_sampler.x, minimum, maximum);


	//Blend the previous and the current ambient occlusion.
	float blended_ambient_occlusion = mix(current_ambient_occlusion_texture_sampler.x, previous_ambient_occlusion_texture_sampler.x, AMBIENT_OCCLUSION_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR + AMBIENT_OCCLUSION_TEMPORAL_DENOISING_BONUS_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	current_ambient_occlusion = vec4(vec3(blended_ambient_occlusion), 1.0f);
	ambient_occlusion = vec4(vec3(blended_ambient_occlusion), 1.0f);
}