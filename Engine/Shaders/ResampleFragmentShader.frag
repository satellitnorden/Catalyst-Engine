//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 delta;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D inputTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Resample.
	vec4 result = vec4(0.0f);
	float total_weight = 0.0f;

	vec2 sample_coordinate = fragment_texture_coordinate + vec2(-1.0f, -1.0f) * delta;
	float sample_weight = float(ValidCoordinate(sample_coordinate));
	result += texture(inputTexture, sample_coordinate) * sample_weight;
	total_weight += sample_weight;

	sample_coordinate = fragment_texture_coordinate + vec2(-1.0f, 1.0f) * delta;
	sample_weight = float(ValidCoordinate(sample_coordinate));
	result += texture(inputTexture, sample_coordinate) * sample_weight;
	total_weight += sample_weight;

	sample_coordinate = fragment_texture_coordinate + vec2(1.0f, -1.0f) * delta;
	sample_weight = float(ValidCoordinate(sample_coordinate));
	result += texture(inputTexture, sample_coordinate) * sample_weight;
	total_weight += sample_weight;

	sample_coordinate = fragment_texture_coordinate + vec2(1.0f, 1.0f) * delta;
	sample_weight = float(ValidCoordinate(sample_coordinate));
	result += texture(inputTexture, sample_coordinate) * sample_weight;
	total_weight += sample_weight;

	//Write the fragment.
	fragment = result / total_weight;
}