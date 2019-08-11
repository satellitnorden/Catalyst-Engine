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

	return texture(velocityTexture, fragmentTextureCoordinate).xy;
}

void main()
{
	//Do just do a passthrough if motion blur is turned off.
	if (motionBlurMode == MOTION_BLUR_MODE_NONE)
	{
		//Write the fragment.
    	fragment = vec4(texture(sceneTexture, fragmentTextureCoordinate).rgb, 1.0f);
	}
	
	else
	{
		//Calculate the motion blur scale.
		float motion_blur_scale = mix(0.5f, 1.0f, float(motionBlurMode == MOTION_BLUR_MODE_FULL));

		//Calculate the blur direction.
		vec2 blurDirection = GetVelocity() * -1.0f * motion_blur_scale;

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
}