//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Out parameters.
layout (location = 0) out int instanceIndex;

void main()
{
    //Pass along the vertex index.
    instanceIndex = gl_InstanceIndex;
} 