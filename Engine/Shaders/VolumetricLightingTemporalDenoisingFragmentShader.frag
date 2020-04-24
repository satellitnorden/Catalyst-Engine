//Constants.
#define VOLUMETRIC_LIGHTING_TEMPORAL_DENOISING_FEEDBACK_FACTOR (0.75f) //0.025f step.

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_4_texture;
layout (set = 1, binding = 1) uniform sampler2D previous_volumetric_lighting_texture;
layout (set = 1, binding = 2) uniform sampler2D current_volumetric_lighting_texture;

//Out parameters.
layout (location = 0) out vec4 current_volumetric_lighting;
layout (location = 1) out vec4 volumetric_lighting;

void main()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjittered_screen_coordinate = fragment_texture_coordinate - currentFrameJitter;

	//Sample the current volumetric lighting texture.
	vec4 current_volumetric_lighting_texture_sampler = texture(current_volumetric_lighting_texture, unjittered_screen_coordinate);

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjittered_screen_coordinate - texture(scene_features_4_texture, unjittered_screen_coordinate).xy;

	//Sample the previous volumetric lighting texture.
	vec4 previous_volumetric_lighting_texture_sampler = texture(previous_volumetric_lighting_texture, previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));


	//Blend the previous and the current volumetric lighting.
	vec3 blended_volumetric_lighting = mix(current_volumetric_lighting_texture_sampler.rgb, previous_volumetric_lighting_texture_sampler.rgb, VOLUMETRIC_LIGHTING_TEMPORAL_DENOISING_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	current_volumetric_lighting = vec4(blended_volumetric_lighting, 1.0f);
	volumetric_lighting = vec4(blended_volumetric_lighting, 1.0f);
}