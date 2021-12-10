//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D PREVIOUS_TEMPORAL_BUFFER_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D AMBIENT_OCCLUSION_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 current_temporal_buffer;
layout (location = 1) out vec4 ambient_occlusion;

/*
*	Constains.
*/
float Constrain(float previous, float minimum, float maximum)
{
	return clamp(previous, minimum, maximum);
}

void CatalystShaderMain()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - CURRENT_FRAME_JITTER;

	//Sample the current ambient occlusion texture.
	float current_ambient_occlusion = texture(AMBIENT_OCCLUSION_TEXTURE, fragment_texture_coordinate).x;

	//Calculate the minimum/maximum ambient occlusion values in the neighborhood of the current frame.
	float minimum = current_ambient_occlusion;
	float maximum = current_ambient_occlusion;

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(x), float(y)) * INVERSE_SCALED_RESOLUTION * 2.0f;
		
			float neighbordhood_sample = texture(AMBIENT_OCCLUSION_TEXTURE, sample_coordinate).x;

			minimum = min(minimum, neighbordhood_sample);
			maximum = max(maximum, neighbordhood_sample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), unjittered_screen_coordinate).xy;

	//Sample the previous ambient occlusion texture.
	float previous_ambient_occlusion = texture(PREVIOUS_TEMPORAL_BUFFER_TEXTURE, previous_screen_coordinate).r;

	//Constraint the previous ambient occlusion.
	previous_ambient_occlusion = Constrain(previous_ambient_occlusion, minimum, maximum);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How closely are the previous sample in the neighborhood of the current sample?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	//previous_sample_weight *= NeighborhoodWeight(previous_ambient_occlusion, minimum, maximum);

	//Blend the previous and the current ambient occlusion.
	float blended_ambient_occlusion = mix(current_ambient_occlusion, previous_ambient_occlusion, previous_sample_weight);

	//Write the fragments.
	current_temporal_buffer = vec4(vec3(blended_ambient_occlusion), 1.0f);
	ambient_occlusion = vec4(vec3(blended_ambient_occlusion), 1.0f);
}