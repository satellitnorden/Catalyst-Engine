//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec4 box;
};

//The vertices.
vec4 vertices[4] = vec4[]
(
	vec4(box.x * 2.0f - 1.0f, (1.0f - box.y) * 2.0f - 1.0f, 1.0f, 1.0f),
	vec4(box.x * 2.0f - 1.0f, (1.0f - box.w) * 2.0f - 1.0f, 1.0f, 1.0f),
	vec4(box.z * 2.0f - 1.0f, (1.0f - box.w) * 2.0f - 1.0f, 1.0f, 1.0f),
	vec4(box.z * 2.0f - 1.0f, (1.0f - box.y) * 2.0f - 1.0f, 1.0f, 1.0f)
);

void main()
{
	gl_Position = vertices[gl_VertexIndex];
}