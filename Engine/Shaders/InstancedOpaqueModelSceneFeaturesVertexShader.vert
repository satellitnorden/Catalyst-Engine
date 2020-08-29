//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) ivec3 WORLD_GRID_DELTA;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;
layout (location = 4) in mat4 vertex_transformation;

//Out parameters.
layout (location = 0) out mat3 fragment_tangent_space_matrix;
layout (location = 3) out vec3 fragment_world_position;
layout (location = 4) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	vec3 tangent = normalize(vec3(vertex_transformation * vec4(vertex_tangent, 0.0f)));
	vec3 bitangent = normalize(vec3(vertex_transformation * vec4(cross(vertex_normal, vertex_tangent), 0.0f)));
	vec3 normal = normalize(vec3(vertex_transformation * vec4(vertex_normal, 0.0f)));

	fragment_tangent_space_matrix = mat3(tangent, bitangent, normal);
	fragment_world_position = vec3(vertex_transformation * vec4(vertex_position, 1.0f)) + vec3(WORLD_GRID_DELTA) * WORLD_GRID_SIZE;
	fragment_texture_coordinate = vertex_texture_coordinate;

	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_world_position, 1.0f);
}