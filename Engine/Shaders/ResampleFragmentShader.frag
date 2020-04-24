//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 delta;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D inputTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Resample.
	vec4 result = vec4(0.0f);

	result += texture(inputTexture, fragmentTextureCoordinate + vec2(-1.0f, -1.0f) * delta);
	result += texture(inputTexture, fragmentTextureCoordinate + vec2(-1.0f, 1.0f) * delta);
	result += texture(inputTexture, fragmentTextureCoordinate + vec2(1.0f, -1.0f) * delta);
	result += texture(inputTexture, fragmentTextureCoordinate + vec2(1.0f, 1.0f) * delta);

	//Write the fragment.
	fragment = result * 0.25f;
}