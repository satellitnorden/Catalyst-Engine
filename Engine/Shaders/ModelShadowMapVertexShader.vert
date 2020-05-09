//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 MODEL_TO_WORLD_MATRIX;
    layout (offset = 64) mat4 WORLD_TO_LIGHT_MATRIX;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;

void CatalystShaderMain()
{
	//Write the position.
	gl_Position = WORLD_TO_LIGHT_MATRIX * MODEL_TO_WORLD_MATRIX * vec4(vertex_position, 1.0f);
}