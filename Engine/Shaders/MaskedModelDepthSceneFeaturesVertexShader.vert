//Push constant data.
layout (push_constant) uniform ModelData
{
    layout (offset = 0) mat4 CURRENT_WORLD_TRANSFORM;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	//Pass data to the fragment shader.
	fragment_texture_coordinate = vertex_texture_coordinate;

	//Calculate the current world position.
	vec3 current_world_position = vec3(CURRENT_WORLD_TRANSFORM * vec4(vertex_position, 1.0));

	//Write the position.
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(current_world_position, 1.0f);
}