//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float DEPTH_OF_FIELD_FOCUS_DISTANCE;
	layout (offset = 4) uint SOURCE_RENDER_TARGET_INDEX;
	layout (offset = 8) int STRIDE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Calculates the depth of field weight at the given coordinate.
*/
float CalculateDepthOfFieldWeight(vec2 coordinate)
{
	float view_distance = -(CalculateViewSpacePosition(coordinate, texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate).w).z);

	return min(view_distance / DEPTH_OF_FIELD_FOCUS_DISTANCE, 1.0f);
}

void CatalystShaderMain()
{
	//Sample the depth of field and depth of field weight at the current fragment.
	vec3 current_depth_of_field = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).rgb;
	float current_depth_of_field_weight = CalculateDepthOfFieldWeight(fragment_texture_coordinate);

	//Sample neighboring fragments.
	vec3 denoised_depth_of_field = vec3(0.0f);
	float total_weight = 0.0f;

	for (int Y = -STRIDE; Y <= STRIDE; Y += STRIDE)
	{
		for (int X = -STRIDE; X <= STRIDE; X += STRIDE)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(X), float(Y)) * INVERSE_SCALED_RESOLUTION * 2.0f;

			vec3 sample_depth_of_field = texture(sampler2D(RENDER_TARGETS[SOURCE_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate).rgb;
			float sample_depth_of_field_weight = CalculateDepthOfFieldWeight(sample_coordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. Is the sample coordinate valid?
			*	2. How closely aligned are the depth of field weights to each other?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));
			sample_weight *= 1.0f - min(abs(current_depth_of_field_weight - sample_depth_of_field_weight), 1.0f);

			denoised_depth_of_field += sample_depth_of_field * sample_weight;
			total_weight += sample_weight;
		}
	}
	
	//Normalize the denoised depth of field. 
	denoised_depth_of_field = total_weight == 0.0f ? current_depth_of_field : denoised_depth_of_field / total_weight;

	//Write the fragment.
	fragment = vec4(denoised_depth_of_field, 1.0f);
}