//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define TEMPORAL_ACCUMULATION_FEEDBACK_FACTOR (0.9f)

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

void main()
{
	//Calculate the unjittered screen coordinate.
	vec2 unjitteredScreenCoordinate = fragmentTextureCoordinate - currentFrameJitter;

	//Sample the scene features 2 texture.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Sample the current frame texture.
	vec4 currentFrameTextureSampler = texture(currentFrameTexture, unjitteredScreenCoordinate);

	//Calculate the world position at this fragment for the current frame.
	vec3 currentWorldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * sceneFeatures2TextureSampler.w;

	//Calculate the previous screen coordinate.
	vec4 previousViewSpacePosition = viewMatrixMinusOne * vec4(currentWorldPosition, 1.0f);
	vec2 previousScreenCoordinate = 0.5f * (previousViewSpacePosition.xy / previousViewSpacePosition.w) + 0.5f;

	//Sample the previous frame texture.
	vec4 previousFrameTextureSampler = texture(previousFrameTexture, previousScreenCoordinate);

	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen?
	*	2. How closely aligned are expected and actual hit distances?
	*/
	float previousSampleWeight = 1.0f;

	previousSampleWeight *= float(ValidCoordinate(previousScreenCoordinate));
	previousSampleWeight *= 1.0f - min(abs(sceneFeatures2TextureSampler.w - previousFrameTextureSampler.w), 1.0f);

	//Blend the previous and the current frame.
	vec3 blendedFrame = mix(currentFrameTextureSampler.rgb, previousFrameTextureSampler.rgb, TEMPORAL_ACCUMULATION_FEEDBACK_FACTOR * previousSampleWeight);

	//Write the fragments.
	currentFrame = vec4(blendedFrame, sceneFeatures2TextureSampler.w);
	scene = vec4(blendedFrame, 1.0f);
}