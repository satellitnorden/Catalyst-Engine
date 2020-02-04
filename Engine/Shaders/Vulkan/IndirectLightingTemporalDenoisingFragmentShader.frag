//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_FEEDBACK_FACTOR (0.99f)
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE (5.0f)
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END ((INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D previous_indirect_lighting_texture;
layout (set = 1, binding = 2) uniform sampler2D current_indirect_lighting_texture;

//Out parameters.
layout (location = 0) out vec4 current_indirect_lighting;
layout (location = 1) out vec4 indirect_lighting;

/*
*	Constrains the point within the minimum/maximum.
*/
vec3 Constrain(vec3 minimum, vec3 maximum, vec3 point)
{
#if 0 //Clamp instead of clip.

	//return clamp(point, minimum, maximum);
	return point;

#else

	vec3 p_clip = 0.5f * (maximum + minimum);
	vec3 e_clip = 0.5f * (maximum - minimum);

	vec3 v_clip = point - p_clip;
	vec3 v_unit = v_clip / e_clip;
	vec3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0f)
		return p_clip + v_clip / ma_unit;
	else
		return point;

#endif
}

void main()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - currentFrameJitter;

	//Sample the current indirect lighting texture.
	vec4 current_indirect_lighting_texture_sampler = texture(current_indirect_lighting_texture, unjittered_screen_coordinate);

	//Calculate the minimum/maximum color values in the neighborhood of the current frame.
	vec3 minimum = current_indirect_lighting_texture_sampler.rgb;
	vec3 maximum = current_indirect_lighting_texture_sampler.rgb;

	for (float x = -INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; x <= INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++x)
	{
		for (float y = -INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; y <= INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END; ++y)
		{
			vec2 sample_coordinate = unjittered_screen_coordinate + vec2(x, y) * inverseScaledResolution;
		
			vec3 neighbordhood_sample = texture(current_indirect_lighting_texture, sample_coordinate).rgb;

			minimum = min(minimum, neighbordhood_sample);
			maximum = max(maximum, neighbordhood_sample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(scene_features_2_texture, unjittered_screen_coordinate).yz;

	//Sample the previous indirect lighting texture.
	vec4 previous_indirect_lighting_texture_sampler = texture(previous_indirect_lighting_texture, previous_screen_coordinate);

	//Constrain the previous frame sample.
	previous_indirect_lighting_texture_sampler.rgb = Constrain(minimum, maximum, previous_indirect_lighting_texture_sampler.rgb);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));

	//Blend the previous and the current indirect lighting.
	vec3 blended_indirect_lighting = mix(current_indirect_lighting_texture_sampler.rgb, previous_indirect_lighting_texture_sampler.rgb, INDIRECT_LIGHTING_TEMPORAL_DENOISING_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	current_indirect_lighting = vec4(blended_indirect_lighting, 1.0f);
	indirect_lighting = vec4(blended_indirect_lighting, 1.0f);
}