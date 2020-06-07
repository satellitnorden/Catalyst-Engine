//Push constant data.
layout (push_constant) uniform ModelData
{
    layout (offset = 0) mat4 PREVIOUS_WORLD_TRANSFORM;
    layout (offset = 64) mat4 CURRENT_WORLD_TRANSFORM;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out mat3 fragment_tangent_space_matrix;
layout (location = 3) out vec3 fragment_previous_world_position;
layout (location = 4) out vec3 fragment_current_world_position;
layout (location = 5) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	//Calculate the tangent space matrix.
	vec3 tangent = normalize(vec3(CURRENT_WORLD_TRANSFORM * vec4(vertex_tangent, 0.0f)));
	vec3 bitangent = normalize(vec3(CURRENT_WORLD_TRANSFORM * vec4(cross(vertex_normal, vertex_tangent), 0.0f)));
	vec3 normal = normalize(vec3(CURRENT_WORLD_TRANSFORM * vec4(vertex_normal, 0.0f)));

	//Pass data to the fragment shader.
	fragment_tangent_space_matrix = mat3(tangent, bitangent, normal);
	fragment_previous_world_position = vec3(PREVIOUS_WORLD_TRANSFORM * vec4(vertex_position, 1.0));
	fragment_current_world_position = vec3(CURRENT_WORLD_TRANSFORM * vec4(vertex_position, 1.0));
	fragment_texture_coordinate = vertex_texture_coordinate;

	//Write the position.
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_current_world_position, 1.0f);
}