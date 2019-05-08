//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define MOTION_BLUR_SAMPLES (16)
#define MOTION_BLUR_SCALE (1.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeaturesTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Calculate the world position at this fragment the current frame.
	vec3 worldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * texture(sceneFeaturesTexture, fragmentTextureCoordinate).w;

	//Now calculate it's previous screen position using the previous view matrix.
	vec4 previousViewSpacePosition = previousViewMatrix * vec4(worldPosition, 1.0f);

	//Perform perspective division.
	float inversePerspectiveDenominator = 1.0f / previousViewSpacePosition.w;
	previousViewSpacePosition.xyz *= inversePerspectiveDenominator;

	vec2 previousScreenPosition = (previousViewSpacePosition.xy + 1.0f) * 0.5f;

	//Now calculate the blur direction!.
	vec2 blurDirection = (fragmentTextureCoordinate - previousScreenPosition) * MOTION_BLUR_SCALE;

	//Calculate the blur step.
	vec2 blurStep = blurDirection / MOTION_BLUR_SAMPLES;

	//Perform the blur.
	vec3 blurred = vec3(0.0f);

	for (int i = 0; i < MOTION_BLUR_SAMPLES; ++i)
	{
		blurred += texture(sceneTexture, fragmentTextureCoordinate + blurStep * i).rgb;
	}

	blurred /= MOTION_BLUR_SAMPLES;

    //Write the fragment.
    fragment = vec4(blurred, 1.0f);
}