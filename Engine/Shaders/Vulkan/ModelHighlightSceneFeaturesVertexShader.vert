//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform ModelData
{
    layout (offset = 0) mat4 current_model_matrix;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out mat3 fragment_tangent_space_matrix;
layout (location = 3) out vec3 fragment_current_world_position;
layout (location = 4) out vec2 fragment_texture_coordinate;

void main()
{
	vec3 tangent = normalize(vec3(current_model_matrix * vec4(vertex_tangent, 0.0f)));
	vec3 bitangent = normalize(vec3(current_model_matrix * vec4(cross(vertex_normal, vertex_tangent), 0.0f)));
	vec3 normal = normalize(vec3(current_model_matrix * vec4(vertex_normal, 0.0f)));

	fragment_tangent_space_matrix = mat3(tangent, bitangent, normal);
	fragment_current_world_position = vec3(current_model_matrix * vec4(vertex_position, 1.0));
	fragment_texture_coordinate = vertex_texture_coordinate;

	gl_Position = viewMatrix * vec4(fragment_current_world_position, 1.0f);
}