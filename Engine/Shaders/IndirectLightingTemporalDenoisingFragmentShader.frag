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
*	Constrains the previous sample.
*/
vec3 Constrain(vec3 previous, vec3 minimum, vec3 maximum)
{
	vec3 p_clip = 0.5f * (maximum + minimum);
	vec3 e_clip = 0.5f * (maximum - minimum);

	vec3 v_clip = previous - p_clip;
	vec3 v_unit = v_clip / e_clip;
	vec3 a_unit = abs(v_unit);

	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0f)
		return p_clip + v_clip / ma_unit;
	else
		return previous;
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
	vec2 previous_screen_coordinate = fragment_texture_coordinate - currentFrameJitter - PREVIOUS_FRAME_JITTER - texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).xy;

	//Sample the previous indirect lighting texture.
	vec4 previous_indirect_lighting_texture_sampler = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX_2], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), previous_screen_coordinate);

	//Constrain the previous sample.
	vec3 constrained_previous_sample = Constrain(previous_indirect_lighting_texture_sampler.rgb, minimum, maximum);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How closely are the previous sample in the neighborhood of the current sample?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));

	//Blend the previous and the current indirect lighting.
	vec3 blended_indirect_lighting = vec3(0.0f);

	blended_indirect_lighting += previous_indirect_lighting_texture_sampler.rgb * INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR;
	blended_indirect_lighting += mix(current_indirect_lighting_texture_sampler.rgb, constrained_previous_sample, previous_sample_weight) * (1.0f - INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR);

	//Write the fragments.
	current_indirect_lighting = vec4(blended_indirect_lighting, 1.0f);
	indirect_lighting = vec4(blended_indirect_lighting, 1.0f);
}