//Constants.
#define MOTION_BLUR_SAMPLES (2)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) float MOTION_BLUR_INTENSITY;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_4_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Returns the velocity.
*/
vec2 GetVelocity()
{
	return texture(scene_features_4_texture, fragment_texture_coordinate).xy;
}

void CatalystShaderMain()
{
	//Do just do a passthrough if motion blur is turned off.
	if (MOTION_BLUR_INTENSITY == 0.0f)
	{
		//Write the fragment.
    	fragment = vec4(texture(scene_texture, fragment_texture_coordinate).rgb, 1.0f);
	}
	
	else
	{
		//Calculate the blur direction.
		vec2 blur_direction = GetVelocity() * -1.0f * MOTION_BLUR_INTENSITY;

		//Calculate the offsets.
		float offsets[MOTION_BLUR_SAMPLES];

		vec4 noise_texture_sample_1 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);
#if MOTION_BLUR_SAMPLES > 4
		vec4 noise_texture_sample_2 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 1);
#endif
#if MOTION_BLUR_SAMPLES > 8
		vec4 noise_texture_sample_3 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 2);
#endif
#if MOTION_BLUR_SAMPLES > 12
		vec4 noise_texture_sample_4 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 3);
#endif
#if MOTION_BLUR_SAMPLES > 16
		vec4 noise_texture_sample_5 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 4);
#endif
#if MOTION_BLUR_SAMPLES > 20
		vec4 noise_texture_sample_6 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 5);
#endif
#if MOTION_BLUR_SAMPLES > 24
		vec4 noise_texture_sample_7 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 6);
#endif
#if MOTION_BLUR_SAMPLES > 28
		vec4 noise_texture_sample_8 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 7);
#endif

		offsets[0] = noise_texture_sample_1[0];
#if MOTION_BLUR_SAMPLES >= 2
		offsets[1] = noise_texture_sample_1[1];
#endif
#if MOTION_BLUR_SAMPLES >= 3
		offsets[2] = noise_texture_sample_1[2];
#endif
#if MOTION_BLUR_SAMPLES >= 4
		offsets[3] = noise_texture_sample_1[3];
#endif
#if MOTION_BLUR_SAMPLES >= 5
		offsets[4] = noise_texture_sample_2[0];
#endif
#if MOTION_BLUR_SAMPLES >= 6
		offsets[5] = noise_texture_sample_2[1];
#endif
#if MOTION_BLUR_SAMPLES >= 7
		offsets[6] = noise_texture_sample_2[2];
#endif
#if MOTION_BLUR_SAMPLES >= 8
		offsets[7] = noise_texture_sample_2[3];
#endif
#if MOTION_BLUR_SAMPLES >= 9
		offsets[8] = noise_texture_sample_3[0];
#endif
#if MOTION_BLUR_SAMPLES >= 10
		offsets[9] = noise_texture_sample_3[1];
#endif
#if MOTION_BLUR_SAMPLES >= 11
		offsets[10] = noise_texture_sample_3[2];
#endif
#if MOTION_BLUR_SAMPLES >= 12
		offsets[11] = noise_texture_sample_3[3];
#endif
#if MOTION_BLUR_SAMPLES >= 13
		offsets[12] = noise_texture_sample_4[0];
#endif
#if MOTION_BLUR_SAMPLES >= 14
		offsets[13] = noise_texture_sample_4[1];
#endif
#if MOTION_BLUR_SAMPLES >= 15
		offsets[14] = noise_texture_sample_4[2];
#endif
#if MOTION_BLUR_SAMPLES >= 16
		offsets[15] = noise_texture_sample_4[3];
#endif
#if MOTION_BLUR_SAMPLES >= 17
		offsets[16] = noise_texture_sample_5[0];
#endif
#if MOTION_BLUR_SAMPLES >= 18
		offsets[17] = noise_texture_sample_5[1];
#endif
#if MOTION_BLUR_SAMPLES >= 19
		offsets[18] = noise_texture_sample_5[2];
#endif
#if MOTION_BLUR_SAMPLES >= 20
		offsets[19] = noise_texture_sample_5[3];
#endif
#if MOTION_BLUR_SAMPLES >= 21
		offsets[20] = noise_texture_sample_6[0];
#endif
#if MOTION_BLUR_SAMPLES >= 22
		offsets[21] = noise_texture_sample_6[1];
#endif
#if MOTION_BLUR_SAMPLES >= 23
		offsets[22] = noise_texture_sample_6[2];
#endif
#if MOTION_BLUR_SAMPLES >= 24
		offsets[23] = noise_texture_sample_6[3];
#endif
#if MOTION_BLUR_SAMPLES >= 25
		offsets[24] = noise_texture_sample_7[0];
#endif
#if MOTION_BLUR_SAMPLES >= 26
		offsets[25] = noise_texture_sample_7[1];
#endif
#if MOTION_BLUR_SAMPLES >= 27
		offsets[26] = noise_texture_sample_7[2];
#endif
#if MOTION_BLUR_SAMPLES >= 28
		offsets[27] = noise_texture_sample_7[3];
#endif
#if MOTION_BLUR_SAMPLES >= 29
		offsets[28] = noise_texture_sample_8[0];
#endif
#if MOTION_BLUR_SAMPLES >= 30
		offsets[29] = noise_texture_sample_8[1];
#endif
#if MOTION_BLUR_SAMPLES >= 31
		offsets[30] = noise_texture_sample_8[2];
#endif
#if MOTION_BLUR_SAMPLES >= 32
		offsets[31] = noise_texture_sample_8[3];
#endif

		//Calculate the blurred scene.
		vec3 blurred_scene = vec3(0.0f);

		for (uint i = 0; i < MOTION_BLUR_SAMPLES; ++i)
		{
			blurred_scene += texture(scene_texture, fragment_texture_coordinate + blur_direction * offsets[i]).rgb;
		}

		blurred_scene /= MOTION_BLUR_SAMPLES;

	    //Write the fragment.
	    fragment = vec4(blurred_scene, 1.0f);
	}
}