//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D indirectLightingTexture;
layout (set = 1, binding = 1) uniform sampler2D directLightingTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;


void main()
{
	//Write the fragment.
	fragment = texture(indirectLightingTexture, fragmentTextureCoordinate) + texture(directLightingTexture, fragmentTextureCoordinate);
}