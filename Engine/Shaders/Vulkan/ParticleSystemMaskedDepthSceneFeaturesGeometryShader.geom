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
layout (location = 1) in vec2 geometry_scales[];
layout (location = 2) in float geometry_times[];

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;
layout (location = 1) out float fragment_opacity;

#define fade_time (0.25f)
#define lifetime (10.0f)

void main()
{
	//Retrieve the world position.
	vec3 world_position = geometry_positions[0];

	//Retrieve the scale.
	vec2 scale = geometry_scales[0];

	//Retrieve the time.
	float time = geometry_times[0];

	//Calculate the forward, up and right vectors.
	vec3 forward_vector = normalize(PERCEIVER_WORLD_POSITION - world_position);
	vec3 up_vector = vec3(0.0f, 1.0f, 0.0f);
	vec3 right_vector = normalize(cross(forward_vector, up_vector));
	
	//Calculate the opacity.
	float opacity = min(time / fade_time, 1.0f) * clamp(1.0f - ((time - (lifetime - fade_time)) / fade_time), 0.0f, 1.0f);

	//Construct all the vertices.
	fragment_texture_coordinate = vec2(0.0f, 1.0f);
	fragment_opacity = opacity;
	gl_Position = viewMatrix * vec4(world_position - right_vector * (scale.x * 0.5f) - up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_texture_coordinate = vec2(0.0f, 0.0f);
	fragment_opacity = opacity;
	gl_Position = viewMatrix * vec4(world_position - right_vector * (scale.x * 0.5f) + up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_texture_coordinate = vec2(1.0f, 1.0f);
	fragment_opacity = opacity;
	gl_Position = viewMatrix * vec4(world_position + right_vector * (scale.x * 0.5f) - up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_texture_coordinate = vec2(1.0f, 0.0f);
	fragment_opacity = opacity;
	gl_Position = viewMatrix * vec4(world_position + right_vector * (scale.x * 0.5f) + up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	EndPrimitive();
}