//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//In parameters.
layout (location = 0) in vec3 geometry_positions[];
layout (location = 1) in vec3 geometry_velocities[];
layout (location = 2) in vec2 geometry_scales[];

//Out parameters.
layout (location = 0) out vec3 fragment_current_world_position;
layout (location = 1) out vec3 fragment_previous_world_position;
layout (location = 2) out vec3 fragment_normal;
layout (location = 3) out vec2 fragment_texture_coordinate;

void main()
{
	//Retrieve the current world position.
	vec3 current_world_position = geometry_positions[0];

	//Calculate the previous world position.
	vec3 previous_world_position = current_world_position - geometry_velocities[0];

	//Retrieve the scale.
	vec2 scale = geometry_scales[0];

	//Calculate the forward, up and right vectors.
	vec3 forward_vector = normalize(PERCEIVER_WORLD_POSITION - current_world_position);
	vec3 up_vector = vec3(0.0f, 1.0f, 0.0f);
	vec3 right_vector = normalize(cross(forward_vector, up_vector));
	
	//Construct all the vertices.
	fragment_current_world_position = current_world_position;
	fragment_previous_world_position = previous_world_position;
	fragment_normal = forward_vector;
	fragment_texture_coordinate = vec2(0.0f, 1.0f);
	gl_Position = viewMatrix * vec4(current_world_position - right_vector * (scale.x * 0.5f) - up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_current_world_position = current_world_position;
	fragment_previous_world_position = previous_world_position;
	fragment_normal = forward_vector;
	fragment_texture_coordinate = vec2(0.0f, 0.0f);
	gl_Position = viewMatrix * vec4(current_world_position - right_vector * (scale.x * 0.5f) + up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_current_world_position = current_world_position;
	fragment_previous_world_position = previous_world_position;
	fragment_normal = forward_vector;
	fragment_texture_coordinate = vec2(1.0f, 1.0f);
	gl_Position = viewMatrix * vec4(current_world_position + right_vector * (scale.x * 0.5f) - up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_current_world_position = current_world_position;
	fragment_previous_world_position = previous_world_position;
	fragment_normal = forward_vector;
	fragment_texture_coordinate = vec2(1.0f, 0.0f);
	gl_Position = viewMatrix * vec4(current_world_position + right_vector * (scale.x * 0.5f) + up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	EndPrimitive();
}