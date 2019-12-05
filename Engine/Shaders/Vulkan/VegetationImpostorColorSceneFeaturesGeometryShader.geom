//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float impostor_half_width;
    layout (offset = 4) float impostor_height;
};

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//In parameters.
layout (location = 0) in vec3 geometry_positions[];

//Out parameters.
layout (location = 0) out vec3 fragment_normal;
layout (location = 1) out vec2 fragment_texture_coordinate;
layout (location = 2) out float fragment_hit_distance;

void main()
{
	//Retrieve the world position.
	vec3 world_position = geometry_positions[0];

	//Calculate the hit distance.
	const float hit_distance = length(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the forward, up and right vectors.
	vec3 forward_vector = PERCEIVER_WORLD_POSITION - world_position;
	forward_vector.y = 0.0f;
	forward_vector = normalize(forward_vector);
	vec3 up_vector = vec3(0.0f, 1.0f, 0.0f);
	vec3 right_vector = normalize(cross(forward_vector, up_vector));
	
	//Construct all the vertices.
	fragment_normal = forward_vector;
	fragment_texture_coordinate = vec2(0.0f, 1.0f);
	fragment_hit_distance = hit_distance;
	gl_Position = viewMatrix * vec4(world_position - right_vector * impostor_half_width, 1.0f);

	EmitVertex();

	fragment_normal = up_vector;
	fragment_texture_coordinate = vec2(0.0f, 0.0f);
	fragment_hit_distance = hit_distance;
	gl_Position = viewMatrix * vec4(world_position - right_vector * impostor_half_width + up_vector * impostor_height, 1.0f);

	EmitVertex();

	fragment_normal = forward_vector;
	fragment_texture_coordinate = vec2(1.0f, 1.0f);
	fragment_hit_distance = hit_distance;
	gl_Position = viewMatrix * vec4(world_position + right_vector * impostor_half_width, 1.0f);

	EmitVertex();

	fragment_normal = up_vector;
	fragment_texture_coordinate = vec2(1.0f, 0.0f);
	fragment_hit_distance = hit_distance;
	gl_Position = viewMatrix * vec4(world_position + right_vector * impostor_half_width + up_vector * impostor_height, 1.0f);

	EmitVertex();

	EndPrimitive();
}