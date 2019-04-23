//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout(location = 1) rayPayloadInNV float visibility;

void main()
{
    visibility = 1.0f;
}