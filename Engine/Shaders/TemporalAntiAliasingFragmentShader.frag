//Constants.
#define TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR (0.99f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float WEIGHT_OVERRIDE;
	layout (offset = 4) float WEIGHT_OVERRIDE_WEIGHT;
};

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_4_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D PREVIOUS_FRAME_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D CURRENT_FRAME_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 current_frame;
layout (location = 1) out vec4 scene;

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
	if (WEIGHT_OVERRIDE_WEIGHT == 0.0f)
	{
		//Calculate the unjittered screen coordinate.
		vec2 unjittered_screen_coordinate = fragment_texture_coordinate - CURRENT_FRAME_JITTER;

		//Sample the current frame texture.
		vec4 current_frame_texture_sampler = texture(CURRENT_FRAME_TEXTURE, unjittered_screen_coordinate);

		//Calculate the minimum/maximum color values in the neighborhood of the current frame. Also find the velocity of the closest fragment.
		float closest_depth = 0.0f;
		vec2 closest_velocity = texture(SCENE_FEATURES_4_TEXTURE, unjittered_screen_coordinate).xy;
		vec3 minimum = current_frame_texture_sampler.rgb;
		vec3 maximum = current_frame_texture_sampler.rgb;

		for (int Y = -1; Y <= 1; ++Y)
		{
			for (int X = -1; X <= 1; ++X)
			{
				vec2 sample_coordinate = unjittered_screen_coordinate + vec2(float(X), float(Y)) * INVERSE_SCALED_RESOLUTION;
			
				vec3 neighbordhood_sample = texture(CURRENT_FRAME_TEXTURE, sample_coordinate).rgb;

				minimum = min(minimum, neighbordhood_sample);
				maximum = max(maximum, neighbordhood_sample);

				float neighborhood_depth = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_coordinate).w;

				if (closest_depth < neighborhood_depth)
				{
					closest_depth = neighborhood_depth;
					closest_velocity = texture(SCENE_FEATURES_4_TEXTURE, sample_coordinate).xy;
				}
			}
		}

		//Calculate the previous screen coordinate.
		vec2 previous_screen_coordinate = unjittered_screen_coordinate - closest_velocity;

		//Sample the previous frame texture.
		vec4 previous_frame_texture_sampler = texture(PREVIOUS_FRAME_TEXTURE, previous_screen_coordinate);

		//Constrain the previous sample.
		previous_frame_texture_sampler.rgb = Constrain(previous_frame_texture_sampler.rgb, minimum, maximum);

		/*
		*	Calculate the weight between the current frame and the history depending on certain criteria.
		*
		*	1. Is the sample coordinate valid?
		*/
		float previous_sample_weight = 1.0f;

		previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));

		//Calculate the final weight.
		float final_weight = TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR * previous_sample_weight;

		//Blend the previous and the current frame.
		vec3 blended_frame = mix(current_frame_texture_sampler.rgb, previous_frame_texture_sampler.rgb, final_weight);

		//Write the fragments.
		current_frame = vec4(blended_frame, 1.0f);
		scene = vec4(blended_frame, 1.0f);
	}

	else
	{
		//Sample the previous frame texture.
		vec4 previous_frame_texture_sampler = texture(PREVIOUS_FRAME_TEXTURE, fragment_texture_coordinate);

		//Sample the current frame texture.
		vec4 current_frame_texture_sampler = texture(current_frame_texture, fragment_texture_coordinate);

		//Blend the previous and the current frame.
		vec3 blended_frame = mix(current_frame_texture_sampler.rgb, previous_frame_texture_sampler.rgb, WEIGHT_OVERRIDE);

		//Write the fragments.
		current_frame = vec4(blended_frame, 1.0f);
		scene = vec4(blended_frame, 1.0f);
	}
}