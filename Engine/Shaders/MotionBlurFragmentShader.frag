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

		//Sample the noise texture(s).
		vec4 noise_texture_1 = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

		//Calculate the blurred scene.
		vec3 blurred_scene = texture(scene_texture, fragment_texture_coordinate + blur_direction * noise_texture_1.x).rgb;

	    //Write the fragment.
	    fragment = vec4(blurred_scene, 1.0f);
	}
}