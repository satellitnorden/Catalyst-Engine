//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_velocity;
layout (location = 2) in vec2 vertex_scale;
layout (location = 3) in float vertex_time;

//Out parameters.
layout (location = 0) out vec3 geometry_position;
layout (location = 1) out vec2 geometry_scale;
layout (location = 2) out float geometry_time;

void main()
{
	//Nothing to do here except pass data along to the geometry shader.
	geometry_position = vertex_position;
	geometry_scale = vertex_scale;
	geometry_time = vertex_time;
}