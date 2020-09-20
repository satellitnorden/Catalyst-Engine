//Constants.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR (0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 DELTA;
	layout (offset = 8) uint SOURCE_RENDER_TARGET_INDEX_1;
	layout (offset = 12) uint SOURCE_RENDER_TARGET_INDEX_2;
	layout (offset = 16) uint SCENE_FEATURES_4_TARGET_INDEX;
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
	weight *= 1.0f - clamp(previous.x - maximum.x, 0.0f, 1.0f);
	weight *= 1.0f - clamp(minimum.y - previous.y, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.y - maximum.y, 0.0f, 1.0f);
	weight *= 1.0f - clamp(minimum.z - previous.z, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.z - maximum.z, 0.0f, 1.0f);

	//Bias the weight.
	weight = weight * weight;

	//Return the weight.
	return weight;
}

void CatalystShaderMain()
{
	//Sample the current indirect lighting texture.
	vec4 current_indirect_lighting_texture_sampler = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX_1], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Calculate the minimum/maximum color values in the neighborhood of the current frame.
	vec3 minimum = current_indirect_lighting_texture_sampler.rgb;
	vec3 maximum = current_indirect_lighting_texture_sampler.rgb;

	for (int Y = -1; Y <= 1; ++Y)
	{
		for (int X = -1; X <= 1; ++X)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(X), float(Y)) * DELTA;
		
			vec4 neighbordhood_sample = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX_1], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate);

			minimum = min(minimum, neighbordhood_sample.rgb);
			maximum = max(maximum, neighbordhood_sample.rgb);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = fragment_texture_coordinate - CURRENT_FRAME_JITTER - PREVIOUS_FRAME_JITTER - texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).xy;

	//Sample the previous indirect lighting texture.
	vec4 previous_indirect_lighting_texture_sampler = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX_2], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How closely are the previous sample in the neighborhood of the current sample?
	*	3. What is the hit percent of the previous sample=
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= NeighborhoodWeight(previous_indirect_lighting_texture_sampler.rgb, minimum, maximum);
	previous_sample_weight *= previous_indirect_lighting_texture_sampler.a;

	//Blend the previous and the current indirect lighting.
	vec4 blended_indirect_lighting = mix(current_indirect_lighting_texture_sampler, previous_indirect_lighting_texture_sampler, INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR + previous_sample_weight * (0.99f - INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR));

	//Write the fragments.
	current_indirect_lighting = blended_indirect_lighting;
	indirect_lighting = blended_indirect_lighting;
}