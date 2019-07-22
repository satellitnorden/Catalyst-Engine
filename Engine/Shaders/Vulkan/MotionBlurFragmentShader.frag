//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define MOTION_BLUR_SCALE (1.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D velocityTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Returns the velocity.
*/
vec2 GetVelocity()
{
	/*
	vec2 chosenVelocity = vec2(0.0f);
	float closestHitDistance = viewDistance;

	for (float x = -1.0f; x <= 1.0f; ++x)
	{
		for (float y = -1.0f; y <= 1.0f; ++y)
		{
			vec2 sampleCoordinate = vec2(x, y) * inverseScaledResolution;

			float hitDistance = texture(sceneFeatures2Texture, sampleCoordinate).w;

			if (closestHitDistance > hitDistance)
			{
				closestHitDistance = hitDistance;
				chosenVelocity = texture(velocityTexture, sampleCoordinate).xy;
			}
		}
	}

	return chosenVelocity;
	*/

	return texture(velocityTexture, fragmentTextureCoordinate).xy;
}

void main()
{
	//Calculate the blur direction.
	vec2 blurDirection = GetVelocity() * -1.0f * MOTION_BLUR_SCALE;

	//Sample the active noise texture.
	vec4 activeNoiseTexture = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

	//Calculate the blurred scene.
	vec3 blurredScene = (texture(sceneTexture, fragmentTextureCoordinate + blurDirection * activeNoiseTexture.x).rgb +
						texture(sceneTexture, fragmentTextureCoordinate + blurDirection * activeNoiseTexture.y).rgb +
						texture(sceneTexture, fragmentTextureCoordinate + blurDirection * activeNoiseTexture.z).rgb +
						texture(sceneTexture, fragmentTextureCoordinate + blurDirection * activeNoiseTexture.w).rgb) * 0.25f;

    //Write the fragment.
    fragment = vec4(blurredScene, 1.0f);
}