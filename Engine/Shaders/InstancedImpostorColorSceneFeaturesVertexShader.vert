//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) float HALF_WIDTH;
    layout (offset = 20) float WHOLE_WIDTH;
    layout (offset = 24) float HEIGHT;
    layout (offset = 28) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;

//Out parameters.
layout (location = 0) out vec3 fragment_world_position;
layout (location = 1) out vec3 fragment_normal;
layout (location = 2) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	//Calculate X and Y components.
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);

    //Calculate the world position.
    vec3 world_position = vertex_position + WORLD_GRID_DELTA;

    //Calculate the forward vector.
    vec3 forward_vector = PERCEIVER_WORLD_POSITION - world_position;
    forward_vector.y = 0.0f;
    forward_vector = normalize(forward_vector);

    //Calculate the right vector.
    vec3 right_vector = cross(forward_vector, vec3(0.0f, 1.0f, 0.0f));

    //Modify the world position.
    world_position += (right_vector * HALF_WIDTH + -right_vector * WHOLE_WIDTH * x);
    world_position.y += HEIGHT * y;

    //Set the fragment world position.
    fragment_world_position = world_position;

    //Set the fragment normal.
    fragment_normal = forward_vector;

    //Set the fragment texture coordinate.
    fragment_texture_coordinate.x = x;
    fragment_texture_coordinate.y = 1.0f - y;

    //Write the vertex.
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
}