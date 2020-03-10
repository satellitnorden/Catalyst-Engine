//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

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

	//Bias the weight.
	weight = weight * weight * weight * weight;

	//Return the weight.
	return weight;
}

void main()
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
			vec2 sampleCoordinate = unjitteredScreenCoordinate + vec2(x, y) * inverseScaledResolution;
		
			vec3 neighbordhoodSample = texture(currentFrameTexture, sampleCoordinate).rgb;

			minimum = min(minimum, neighbordhoodSample);
			maximum = max(maximum, neighbordhoodSample);
		}
	}

	//Calculate the previous screen coordinate.
	vec2 previous_screen_coordinate = unjitteredScreenCoordinate - texture(scene_features_4_texture, unjitteredScreenCoordinate).xy;

	//Sample the previous frame texture.
	vec4 previousFrameTextureSampler = texture(previousFrameTexture, previous_screen_coordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	2. How far apart is the color from the minimum/maximum neighborhood?
	*/
	float previous_sample_weight = 1.0f;

	previous_sample_weight *= float(ValidCoordinate(previous_screen_coordinate));
	previous_sample_weight *= NeighborhoodWeight(minimum, maximum, previousFrameTextureSampler.rgb);

	//Blend the previous and the current frame.
	vec3 blended_frame = mix(currentFrameTextureSampler.rgb, previousFrameTextureSampler.rgb, TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR * previous_sample_weight);

	//Write the fragments.
	currentFrame = vec4(blended_frame, 1.0f);
	scene = vec4(blended_frame, 1.0f);
}