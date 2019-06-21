//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 delta;
	layout (offset = 8) int passes;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D inputTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Resample.
	vec4 result = vec4(0.0f);

	for (int i = 0; i < passes; ++i)
	{
		result += texture(inputTexture, fragmentTextureCoordinate + vec2(-1.0f, -1.0f) * vec2(1 + i * 1.5f, 1 + i * 1.5f) * delta);
		result += texture(inputTexture, fragmentTextureCoordinate + vec2(-1.0f, 1.0f) * vec2(1 + i * 1.5f, 1 + i * 1.5f) * delta);
		result += texture(inputTexture, fragmentTextureCoordinate + vec2(1.0f, -1.0f) * vec2(1 + i * 1.5f, 1 + i * 1.5f) * delta);
		result += texture(inputTexture, fragmentTextureCoordinate + vec2(1.0f, 1.0f) * vec2(1 + i * 1.5f, 1 + i * 1.5f) * delta);
	}

	//Write the fragment.
	fragment = result / (4 * passes);
}