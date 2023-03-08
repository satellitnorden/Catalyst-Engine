//Constants.
#define TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR (0.9f)

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
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_2_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_4_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D PREVIOUS_FRAME_TEXTURE;
layout (set = 1, binding = 3) uniform sampler2D CURRENT_FRAME_TEXTURE_NEAREST;
layout (set = 1, binding = 4) uniform sampler2D CURRENT_FRAME_TEXTURE_LINEAR;

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
	{
		return p_clip + v_clip / ma_unit;
	}

	else
	
	{
		return previous;
	}
}

void CatalystShaderMain()
{
	if (WEIGHT_OVERRIDE_WEIGHT == 0.0f)
	{
		//Calculate the minimum/maximum color values in the neighborhood of the current frame. Also find the velocity of the closest fragment.
		float closest_depth = 0.0f;
		vec2 closest_velocity = vec2(0.0f);
		vec3 minimum = vec3(1.0f);
		vec3 maximum = vec3(0.0f);

		vec3 current_frame_texture_sampler = vec3(0.0f);

		for (int Y = -1; Y <= 1; ++Y)
		{
			for (int X = -1; X <= 1; ++X)
			{
				vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(X), float(Y)) * INVERSE_SCALED_RESOLUTION;
			
				vec3 neighbordhood_sample = texture(CURRENT_FRAME_TEXTURE_NEAREST, sample_coordinate).rgb;

				minimum = min(minimum, neighbordhood_sample);
				maximum = max(maximum, neighbordhood_sample);

				float neighborhood_depth = texture(SCENE_FEATURES_2_TEXTURE, sample_coordinate).w;

				if (closest_depth < neighborhood_depth)
				{
					closest_depth = neighborhood_depth;
					closest_velocity = texture(SCENE_FEATURES_4_TEXTURE, sample_coordinate).xy;
				}

				current_frame_texture_sampler += neighbordhood_sample * float(X == 0 && Y == 0);
			}
		}

		//Calculate the previous screen coordinate.
		vec2 previous_screen_coordinate = fragment_texture_coordinate - closest_velocity;

		//Sample the previous frame texture.
		vec3 previous_frame_texture_sampler = texture(PREVIOUS_FRAME_TEXTURE, previous_screen_coordinate).rgb;

		//Constrain the previous sample.
		previous_frame_texture_sampler = Constrain(previous_frame_texture_sampler, minimum, maximum);

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
		vec3 blended_frame = mix(current_frame_texture_sampler, previous_frame_texture_sampler, final_weight);

		//Write the fragments.
		current_frame = vec4(blended_frame, 1.0f);
		scene = vec4(blended_frame, 1.0f);
	}

	else
	{
		//Sample the previous frame texture.
		vec4 previous_frame_texture_sampler = texture(PREVIOUS_FRAME_TEXTURE, fragment_texture_coordinate);

		//Sample the current frame texture.
		vec4 current_frame_texture_sampler = texture(CURRENT_FRAME_TEXTURE_NEAREST, fragment_texture_coordinate);

		//Blend the previous and the current frame.
		vec3 blended_frame = mix(current_frame_texture_sampler.rgb, previous_frame_texture_sampler.rgb, WEIGHT_OVERRIDE);

		//Write the fragments.
		current_frame = vec4(blended_frame, 1.0f);
		scene = vec4(blended_frame, 1.0f);
	}
}