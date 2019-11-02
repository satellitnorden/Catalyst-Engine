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
layout (set = 1, binding = 0) uniform sampler2D velocity_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Returns the velocity.
*/
vec2 GetVelocity()
{
	return texture(velocity_texture, fragmentTextureCoordinate).xy;
}

void main()
{
	//Do just do a passthrough if motion blur is turned off.
	if (motionBlurMode == MOTION_BLUR_MODE_NONE)
	{
		//Write the fragment.
    	fragment = vec4(texture(scene_texture, fragmentTextureCoordinate).rgb, 1.0f);
	}
	
	else
	{
		//Calculate the motion blur scale.
		float motion_blur_scale = mix(0.5f, 1.0f, float(motionBlurMode == MOTION_BLUR_MODE_FULL));

		//Calculate the blur direction.
		vec2 blur_direction = GetVelocity() * -1.0f * motion_blur_scale;

		//Sample the active noise texture.
		vec4 noise_texture_1 = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));
		vec4 noise_texture_2 = texture(sampler2D(globalTextures[(activeNoiseTextureIndex + 1) & 63], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

		//Calculate the blurred scene.
		vec3 blurred_scene = (texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_1.x).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_1.y).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_1.z).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_1.w).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_2.x).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_2.y).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_2.z).rgb +
							texture(scene_texture, fragmentTextureCoordinate + blur_direction * noise_texture_2.w).rgb) * 0.125f;

	    //Write the fragment.
	    fragment = vec4(blurred_scene, 1.0f);
	}
}