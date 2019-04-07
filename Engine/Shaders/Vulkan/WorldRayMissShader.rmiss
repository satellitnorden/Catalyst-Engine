//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

layout(location = 0) rayPayloadInNV vec3 hitValue;

void main()
{
    hitValue = vec3(0.0, 0.1, 0.3);
}