//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) int STRIDE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D depth_of_field_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Sample neighboring fragments.
	vec3 blurred_depth_of_field = vec3(0.0f);

	for (int Y = -STRIDE; Y <= STRIDE; Y += STRIDE)
	{
		for (int X = -STRIDE; X <= STRIDE; X += STRIDE)
		{
			//Calculate the sample coordinate.
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(X), float(Y)) * INVERSE_SCALED_RESOLUTION * 2.0f;

			/*
			*	Calculate the sample weight, depending on these criteria;
			*	
			*	1. Is the sample coordinate a valid coordinate?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));

			blurred_depth_of_field = mix(blurred_depth_of_field, max(blurred_depth_of_field, texture(depth_of_field_texture, sample_coordinate).rgb), sample_weight);
		}
	}
	
	//Write the fragment.
	fragment = vec4(blurred_depth_of_field, 1.0f);
}