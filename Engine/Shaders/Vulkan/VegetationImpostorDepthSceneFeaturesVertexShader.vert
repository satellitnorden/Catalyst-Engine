//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in mat4 transformation;

//Out parameters.
layout (location = 0) out vec3 geometry_position;

void main()
{
	//Nothing to do here except pass the geometry_position.
	geometry_position = transformation[3].xyz;
}