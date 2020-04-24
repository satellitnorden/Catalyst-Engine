//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 direction;
	layout (offset = 8) float size;
	layout (offset = 12) float stride;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D inputTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Calculate the start/end.
	float startAndEnd = (size * 0.5f) * stride;

	//Sample neighboring fragments.
	vec4 blurredFragment = vec4(0.0f);
	float blurredFragmentWeightSum = 0.0f;

	for (float x = -startAndEnd; x <= startAndEnd; x += stride)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * vec2(SQUARE_ROOT_OF_TWO, SQUARE_ROOT_OF_TWO) * direction;

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. Is the sample coordinate valid?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= float(ValidCoordinate(sampleCoordinate));

		blurredFragment += texture(inputTexture, sampleCoordinate) * sampleWeight;
		blurredFragmentWeightSum += sampleWeight;
	}

	//Normalize the blurred fragment.
	blurredFragment /= blurredFragmentWeightSum;

	//Write the fragment.
	fragment = blurredFragment;
}