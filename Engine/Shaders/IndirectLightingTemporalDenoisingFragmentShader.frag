//Includes.
#include "CatalystIndirectLightingCore.glsl"

//Constants.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_BASE_FEEDBACK_FACTOR (0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SOURCE_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_4_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D TEMPORAL_REPROJECTION_BUFFER_TEXTURE;
layout (set = 1, binding = 3) uniform sampler2D PREVIOUS_TEMPORAL_BUFFER;

//Out parameters.
layout (location = 0) out vec4 current_temporal_buffer;
layout (location = 1) out vec4 indirect_lighting;

/*
*	Calculates velocity.
*/
vec2 CalculateVelocity(float temporal_reprojection_buffer_weight)
{
	return mix(texture(SCENE_FEATURES_4_TEXTURE, fragment_texture_coordinate).xy, texture(TEMPORAL_REPROJECTION_BUFFER_TEXTURE, fragment_texture_coordinate).xy, temporal_reprojection_buffer_weight);
}

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
	//Calculate the minimum/maximum color values in the neighborhood of the current frame.
	vec3 minimum = vec3(1.0f);
	vec3 maximum = vec3(0.0f);

	for (int Y = -1; Y <= 1; ++Y)
	{
		for (int X = -1; X <= 1; ++X)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(X), float(Y)) * INVERSE_SCALED_RESOLUTION;
		
			vec4 neighbordhood_sample = texture(SOURCE_TEXTURE, sample_coordinate);

			minimum = min(minimum, neighbordhood_sample.rgb);
			maximum = max(maximum, neighbordhood_sample.rgb);
		}
	}

	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - CURRENT_FRAME_JITTER;

	//Sample the current indirect lighting texture.
	vec4 current_indirect_lighting_texture_sampler = texture(SOURCE_TEXTURE, unjittered_screen_coordinate);

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - CalculateVelocity(current_indirect_lighting_texture_sampler.a);

	//Sample the previous indirect lighting texture.
	vec4 previous_indirect_lighting_texture_sampler = texture(PREVIOUS_TEMPORAL_BUFFER, previous_screen_coordinate);

	//Sample the previous indirect lighting texture.
	previous_indirect_lighting_texture_sampler.rgb = Constrain(previous_indirect_lighting_texture_sampler.rgb, minimum, maximum);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. What is the hit percentage of the previous sample?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= previous_indirect_lighting_texture_sampler.a;

	//Blend the previous and the current indirect lighting.
	vec4 blended_indirect_lighting = mix(current_indirect_lighting_texture_sampler, previous_indirect_lighting_texture_sampler, previous_sample_weight * 0.999f);

	//Write the fragments.
	current_temporal_buffer = blended_indirect_lighting;
	indirect_lighting = blended_indirect_lighting;
}