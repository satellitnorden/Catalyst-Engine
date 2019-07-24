//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR (0.9f)
#define TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_SIZE (3.0f)
#define TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END ((TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D velocityTexture;
layout (set = 1, binding = 1) uniform sampler2D previousFrameTexture;
layout (set = 1, binding = 2) uniform sampler2D currentFrameTexture;

//Out parameters.
layout (location = 0) out vec4 currentFrame;
layout (location = 1) out vec4 scene;

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

	//Calculate the minimum/maximum luminance.
	float minimumLuminance = CalculateAverage(minimum);
	float maximumLuminance = CalculateAverage(maximum);

	//Calculate the previous screen coordinate.
	vec2 previousScreenCoordinate = unjitteredScreenCoordinate - texture(velocityTexture, unjitteredScreenCoordinate).xy;

	//Sample the previous frame texture.
	vec4 previousFrameTextureSampler = texture(previousFrameTexture, previousScreenCoordinate);

	//Calculate the previous frame luminance.
	float previousFrameLuminance = CalculateAverage(previousFrameTextureSampler.rgb);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*	2. How far away is the previous frame sample from the minimum/maximum luminance?
	*/
	float previousSampleWeight = 1.0f;

	previousSampleWeight *= float(ValidCoordinate(previousScreenCoordinate));
	previousSampleWeight *= pow(1.0f - min(max(max(minimumLuminance - previousFrameLuminance, 0.0f), max(previousFrameLuminance - maximumLuminance, 0.0f)), 1.0f), 32.0f);

	//Blend the previous and the current frame.
	vec3 blendedFrame = mix(currentFrameTextureSampler.rgb, previousFrameTextureSampler.rgb, TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR * previousSampleWeight);

	//Write the fragments.
	currentFrame = vec4(blendedFrame, 1.0f);
	if (gl_FragCoord.x > 980.0f || true) scene = vec4(blendedFrame, 1.0f);
	else scene = vec4(texture(velocityTexture, unjitteredScreenCoordinate).x * 10.0f, texture(velocityTexture, unjitteredScreenCoordinate).y * 10.0f, texture(velocityTexture, unjitteredScreenCoordinate).y * 10.0f, 1.0f);
}