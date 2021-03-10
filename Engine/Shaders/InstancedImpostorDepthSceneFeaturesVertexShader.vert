//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	//Calculate X and Y components.
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);

    //Calculate the texture coordinate.
    fragment_texture_coordinate.x = x;
    fragment_texture_coordinate.y = 1.0f - y;

    //Calculate the world position.
    vec3 world_position = vec3(vertex_position.x + (-1.0f + x * 2.0f), vertex_position.y + (y * 2.0f), vertex_position.z);
    
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
}