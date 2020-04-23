//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 32) vec4 COLOR;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec3 fragment_normal;

//Out parameters
layout (location = 0) out vec4 fragment;

void main()
{
	//Calculate the color.
	vec3 color = COLOR.rgb * (dot(fragment_normal, vec3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f);

	//Write the fragment.
    fragment = vec4(color, COLOR.a);
}