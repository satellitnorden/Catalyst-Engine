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
    layout (offset = 0) int accumulations;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D currentRadianceTexture;
layout (set = 1, binding = 1) uniform sampler2D previousRadianceTexture;

//Out parameters.
layout (location = 0) out vec4 currentRadiance;
layout (location = 1) out vec4 previousRadiance;

void main()
{
	//Sample the radiance textures.
	vec4 currentRadianceTextureSampler = texture(currentRadianceTexture, fragmentTextureCoordinate);
	vec4 previousRadianceTextureSampler = texture(previousRadianceTexture, fragmentTextureCoordinate);

	if (accumulations == 0)
	{
		currentRadiance = previousRadiance = currentRadianceTextureSampler;
	}

	else
	{
		previousRadiance = previousRadianceTextureSampler + currentRadianceTextureSampler;
		currentRadiance = previousRadiance / (accumulations + 1);
	}
}