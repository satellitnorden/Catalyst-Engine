//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 INVERSE_RESOLUTION;
	layout (offset = 8) int DIRECTION;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SOURCE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Main function.
*/
void CatalystShaderMain()
{
	//Sample neighboring fragments.
	vec3 brightest_sample = vec3(0.0f);

	for (int sample_position = -1; sample_position <= 1; ++sample_position)
	{
		vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(sample_position) * float(DIRECTION == 0), float(sample_position) * float(DIRECTION == 1)) * INVERSE_RESOLUTION;

		brightest_sample = max(brightest_sample, texture(SOURCE_TEXTURE, sample_coordinate).rgb);
	}
	
	//Write the fragment.
	fragment = vec4(brightest_sample, 1.0f);
}