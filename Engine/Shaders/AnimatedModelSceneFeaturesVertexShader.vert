//Constants.
#define ANIMATED_MODEL_MAXIMUM_BONE_TRANSFORMS (16)
#define ANIMATED_MODEL_MAXIMUM_WEIGHTS (3)

//Push constant data.
layout (push_constant) uniform ModelData
{
    layout (offset = 0) mat4 previous_model_matrix;
    layout (offset = 64) mat4 current_model_matrix;
};

//Bone transforms.
layout (std140, set = 1, binding = 0) uniform BoneTransforms
{
    layout (offset = 0) mat4 bone_transforms[ANIMATED_MODEL_MAXIMUM_BONE_TRANSFORMS];
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in ivec3 vertex_bone_indices;
layout (location = 4) in vec3 vertex_bone_weights;
layout (location = 5) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out mat3 fragment_tangent_space_matrix;
layout (location = 3) out vec3 fragment_previous_world_position;
layout (location = 4) out vec3 fragment_current_world_position;
layout (location = 5) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	vec3 animated_position = vec3(0.0f);
	vec3 animated_normal = vec3(0.0f);
	vec3 animated_tangent = vec3(0.0f);

	for (int i = 0; i < ANIMATED_MODEL_MAXIMUM_WEIGHTS; ++i)
	{
		animated_position += vec4(bone_transforms[vertex_bone_indices[i]] * vec4(vertex_position, 1.0f) * vertex_bone_weights[i]).xyz;
		animated_normal += vec4(bone_transforms[vertex_bone_indices[i]] * vec4(vertex_normal, 0.0f) * vertex_bone_weights[i]).xyz;
		animated_tangent += vec4(bone_transforms[vertex_bone_indices[i]] * vec4(vertex_tangent, 0.0f) * vertex_bone_weights[i]).xyz;
	}

	vec3 tangent = normalize(vec3(current_model_matrix * vec4(animated_tangent, 0.0f)));
	vec3 bitangent = normalize(vec3(current_model_matrix * vec4(cross(animated_normal, animated_tangent), 0.0f)));
	vec3 normal = normalize(vec3(current_model_matrix * vec4(animated_normal, 0.0f)));

	fragment_tangent_space_matrix = mat3(tangent, bitangent, normal);
	fragment_previous_world_position = vec3(previous_model_matrix * vec4(animated_position, 1.0f));
	fragment_current_world_position = vec3(current_model_matrix * vec4(animated_position, 1.0f));
	fragment_texture_coordinate = vertex_texture_coordinate;

	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_current_world_position, 1.0f);
}