//Constants.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR (0.5f) //0.025f step.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_BONUS_FEEDBACK_FACTOR (0.99f - INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR)
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE (3.0f)
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END ((INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint SOURCE_RENDER_TARGET_INDEX;
};

//Out parameters.
layout (location = 0) out vec4 current_indirect_lighting;
layout (location = 1) out vec4 indirect_lighting;

/*
*	Calculates the neighborhood weight.
*/
float NeighborhoodWeight(vec3 previous, vec3 minimum, vec3 maximum)
{
	//Calculate the weight.
	float weight = 1.0f;

	weight *= 1.0f - clamp(minimum.x - previous.x, 0.0f, 1.0f);
	weight *= 1.0f - clamp(minimum.y - previous.y, 0.0f, 1.0f);
	weight *= 1.0f - clamp(minimum.z - previous.z, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.x - maximum.x, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.y - maximum.y, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.z - maximum.z, 0.0f, 1.0f);
	
	//Bias the weight a bit.
	weight = weight * weight;

	//Return the weight.
	return weight;
}

void CatalystShaderMain()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - currentFrameJitter;

	//Sample the current indirect lighting texture.
	vec4 current_indirect_lighting_texture_sampler = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), unjittered_screen_coordinate);

	//Calculate the minimum/maximum color values in the neighborhood of the current frame.
	vec3 minimum = current_indirect_lighting_texture_sampler.rgb;
	vec3 maximum = current_indirect_lighting_texture_sampler.rgb;

	for (float x = -INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; x <= INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++x)
	{
		for (float y = -INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; y <= INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++y)
		{
			vec2 sample_coordinate = unjittered_screen_coordinate + vec2(x, y) * INVERSE_SCALED_RESOLUTION * 2.0f;
		
			vec4 neighbordhood_sample = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate);

			minimum = mix(minimum, min(minimum, neighbordhood_sample.rgb), neighbordhood_sample.a);
			maximum = mix(maximum, max(maximum, neighbordhood_sample.rgb), neighbordhood_sample.a);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), unjittered_screen_coordinate).xy;

	//Sample the previous indirect lighting texture.
	vec4 previous_indirect_lighting_texture_sampler = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How closely are the previous sample in the neighborhood of the current sample?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= NeighborhoodWeight(previous_indirect_lighting_texture_sampler.rgb, minimum, maximum);

	//Blend the previous and the current indirect lighting.
	vec4 blended_indirect_lighting = mix(current_indirect_lighting_texture_sampler, previous_indirect_lighting_texture_sampler, INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR + INDIRECT_LIGHTING_TEMPORAL_DENOISING_BONUS_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	current_indirect_lighting = blended_indirect_lighting;
	indirect_lighting = blended_indirect_lighting;
}