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

void main()
{
	//Calculate the start/end.
	float startAndEnd = (size * 0.5f) * stride;

	//Sample neighboring fragments.
	vec4 blurredFragment = vec4(0.0f);

	for (float x = -startAndEnd; x <= startAndEnd; x += stride)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

		blurredFragment += texture(inputTexture, sampleCoordinate);
	}

	//Write the fragment.
	fragment = blurredFragment / (size + 1.0f);
}