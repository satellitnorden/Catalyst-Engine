//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) float HALF_WIDTH;
    layout (offset = 20) float WHOLE_WIDTH;
    layout (offset = 24) float HEIGHT;
    layout (offset = 28) uint MATERIAL_INDEX;
    layout (offset = 32) float START_FADE_IN_DISTANCE;
    layout (offset = 36) float END_FADE_IN_DISTANCE;
    layout (offset = 40) float START_FADE_OUT_DISTANCE;
    layout (offset = 44) float END_FADE_OUT_DISTANCE;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;
layout (location = 1) out float fragment_fade_opacity;

void CatalystShaderMain()
{
	//Calculate X and Y components.
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);

    //Calculate the texture coordinate.
    fragment_texture_coordinate.x = x;
    fragment_texture_coordinate.y = 1.0f - y;

    //Calculate the world position.
    vec3 world_position = vertex_position + WORLD_GRID_DELTA;

    //Calculate the forward vector.
    vec3 forward_vector = CAMERA_WORLD_POSITION - world_position;

	//Calculate the distance to canera and normalize the forward vector.
    forward_vector.y = 0.0f;
    float distance_to_camera = length(forward_vector);
    float distance_to_camera_reciprocal = 1.0f / distance_to_camera;
    forward_vector *= distance_to_camera_reciprocal;

    //Calculate the right vector.
    vec3 right_vector = cross(forward_vector, vec3(0.0f, 1.0f, 0.0f));

    //Modify the world position.
    world_position += (right_vector * HALF_WIDTH + -right_vector * WHOLE_WIDTH * x);
    world_position.y += HEIGHT * y;

    //Calculate the fragment fade opacity.
    float fade_in_range = END_FADE_IN_DISTANCE - START_FADE_IN_DISTANCE;
    float fade_in = clamp(distance_to_camera - START_FADE_IN_DISTANCE, 0.0f, fade_in_range) / fade_in_range;
    float fade_out_range = END_FADE_OUT_DISTANCE - START_FADE_OUT_DISTANCE;
    float fade_out = 1.0f - (clamp(distance_to_camera - START_FADE_OUT_DISTANCE, 0.0f, fade_out_range) / fade_out_range);

    fragment_fade_opacity = min(fade_in, fade_out);

    //Write the vertex.
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
}