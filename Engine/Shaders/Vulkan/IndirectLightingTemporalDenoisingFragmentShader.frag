//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_FEEDBACK_FACTOR (0.9f) //0.025f step.
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE (3.0f)
#define INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_START_END ((INDIRECT_LIGHTING_TEMPORAL_DENOISING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_4_texture;
layout (set = 1, binding = 1) uniform sampler2D previous_indirect_lighting_texture;
layout (set = 1, binding = 2) uniform sampler2D current_indirect_lighting_texture;

//Out parameters.
layout (location = 0) out vec4 current_indirect_lighting;
layout (location = 1) out vec4 indirect_lighting;

/*
*	Calculates the neighborhood weight.
*/
float NeighborhoodWeight(vec3 minimum, vec3 maximum, vec3 previous)
{
	//Calculate the weight.
	float weight = 1.0f;

	weight *= 1.0f - clamp(minimum.x - previous.x, 0.0f, 1.0f);
	weight *= 1.0f - clamp(minimum.y - previous.y, 0.0f, 1.0f);
	weight *= 1.0f - clamp(minimum.z - previous.z, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.x - maximum.x, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.y - maximum.y, 0.0f, 1.0f);
	weight *= 1.0f - clamp(previous.z - maximum.z, 0.0f, 1.0f);

	//Return the weight.
	return weight;
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
			vec2 sample_coordinate = unjittered_screen_coordinate + vec2(x, y) * inverseScaledResolution * 2.0f;
		
			vec3 neighbordhood_sample = texture(current_indirect_lighting_texture, sample_coordinate).rgb;

			minimum = min(minimum, neighbordhood_sample);
			maximum = max(maximum, neighbordhood_sample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(scene_features_4_texture, unjittered_screen_coordinate).xy;

	//Sample the previous indirect lighting texture.
	vec4 previous_indirect_lighting_texture_sampler = texture(previous_indirect_lighting_texture, previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How far apart is the color from the minimum/maximum neighborhood?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= NeighborhoodWeight(minimum, maximum, previous_indirect_lighting_texture_sampler.rgb);

	//Blend the previous and the current indirect lighting.
	vec4 blended_indirect_lighting = mix(current_indirect_lighting_texture_sampler, previous_indirect_lighting_texture_sampler, INDIRECT_LIGHTING_TEMPORAL_DENOISING_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	current_indirect_lighting = blended_indirect_lighting;
	indirect_lighting = blended_indirect_lighting;
}