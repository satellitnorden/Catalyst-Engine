//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR (0.9f)
#define TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_SIZE (3.0f)
#define TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_START_END ((TEMPORAL_ANTI_ALIASING_NEIGHBORHOOD_SIZE - 1.0f) * 0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D previousFrameTexture;
layout (set = 1, binding = 2) uniform sampler2D currentFrameTexture;

//Out parameters.
layout (location = 0) out vec4 currentFrame;
layout (location = 1) out vec4 scene;

/*
*	Constrains a history sample.
*/
vec3 Constrain(vec3 history, vec3 minimum, vec3 maximum)
{
	//return clamp(history, minimum, maximum);
	return history;
}

void main()
{
	if (antiAliasingMode == ANTI_ALIASING_MODE_TEMPORAL
        || antiAliasingMode == ANTI_ALIASING_MODE_FAST_APPROXIMATE_AND_TEMPORAL)
	{
		//Calculate the unjittered screen coordinate.
		vec2 unjitteredScreenCoordinate = fragmentTextureCoordinate - currentFrameJitter;

		//Sample the scene features 2 texture.
		vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

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

		//Calculate the world position at this fragment for the current frame.
		vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.w;

		//Calculate the previous screen coordinate.
		vec4 previousViewSpacePosition = viewMatrixMinusOne * vec4(currentWorldPosition, 1.0f);
		vec2 previousScreenCoordinate = 0.5f * (previousViewSpacePosition.xy / previousViewSpacePosition.w) + 0.5f;

		//Sample the previous frame texture.
		vec4 previousFrameTextureSampler = texture(previousFrameTexture, previousScreenCoordinate);

		//Calculate the previous frame luminance.
		float previousFrameLuminance = CalculateAverage(previousFrameTextureSampler.rgb);

		//Constrin the previous frame sample.
		vec3 constrainedPreviousFrameSample = Constrain(previousFrameTextureSampler.rgb, minimum, maximum);

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
	    vec3 blendedFrame = mix(currentFrameTextureSampler.rgb, constrainedPreviousFrameSample, TEMPORAL_ANTI_ALIASING_FEEDBACK_FACTOR * previousSampleWeight);

	    //Write the fragments.
	    currentFrame = vec4(blendedFrame, 1.0f);
	    scene = vec4(blendedFrame, 1.0f);
	}

	else
	{
		//Sample the current frame texture.
		vec4 currentFrameTextureSampler = texture(currentFrameTexture, fragmentTextureCoordinate);

		//Write the fragments.
	    currentFrame = vec4(currentFrameTextureSampler.rgb, 1.0f);
	    scene = vec4(currentFrameTextureSampler.rgb, 1.0f);
	}
}