//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D depthOfFieldTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the depth of field texture.
	vec4 depthOfField = Upsample(depthOfFieldTexture, fragmentTextureCoordinate);

    //Write the fragment.
    fragment = depthOfField;
}