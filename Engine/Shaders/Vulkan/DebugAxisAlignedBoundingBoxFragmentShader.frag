//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 64) vec4 color;
};

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{

    //Write the fragment.
    fragment = color;
}