//Constants.
#define TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR (0.99f)
#define TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_SIZE (3.0f)
#define TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END ((TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_4_texture;
layout (set = 1, binding = 1) uniform sampler2D previousFrameTexture;
layout (set = 1, binding = 2) uniform sampler2D currentFrameTexture;

//Out parameters.
layout (location = 0) out vec4 currentFrame;
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
	//Calculate the unjittered screen coordinate.
	vec2 unjitteredScreenCoordinate = fragmentTextureCoordinate - currentFrameJitter;

	//Sample the current frame texture.
	vec4 currentFrameTextureSampler = texture(currentFrameTexture, unjitteredScreenCoordinate);

	//Calculate the minimum/maximum color values in the neighborhood of the current frame.
	vec3 minimum = currentFrameTextureSampler.rgb;
	vec3 maximum = currentFrameTextureSampler.rgb;

	for (float x = -TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END; x <= TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END; ++x)
	{
		for (float y = -TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END; y <= TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END; ++y)
		{
			vec2 sampleCoordinate = unjitteredScreenCoordinate + vec2(x, y) * INVERSE_SCALED_RESOLUTION;
		
			vec3 neighbordhoodSample = texture(currentFrameTexture, sampleCoordinate).rgb;

			minimum = min(minimum, neighbordhoodSample);
			maximum = max(maximum, neighbordhoodSample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjitteredScreenCoordinate - texture(scene_features_4_texture, unjitteredScreenCoordinate).xy;

	//Sample the previous frame texture.
	vec4 previousFrameTextureSampler = texture(previousFrameTexture, previous_screen_coordinate);

	//Constrain the previous sample.
	previousFrameTextureSampler.rgb = Constrain(previousFrameTextureSampler.rgb, minimum, maximum);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the sample coordinate valid?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));

	//Blend the previous and the current frame.
	vec3 blended_frame = mix(currentFrameTextureSampler.rgb, previousFrameTextureSampler.rgb, TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	currentFrame = vec4(blended_frame, 1.0f);
	scene = vec4(blended_frame, 1.0f);
}