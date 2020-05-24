//Push constant data.
layout(push_constant) uniform PushConstantData
{
    vec2 SCALE;
    vec2 TRANSLATION;
};

//In parameters.
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_texture_coordinate;
layout(location = 2) in vec4 vertex_color;

//Out parameters.
layout (location = 0) out vec4 fragment_color;
layout (location = 1) out vec2 fragment_texture_coordinate;

void main()
{
	//Pass data to the fragment shader.
    fragment_color = vertex_color;
    fragment_texture_coordinate = vertex_texture_coordinate;

    //Write the position.
    gl_Position = vec4(vertex_position * SCALE + TRANSLATION, 0.0f, 1.0f);
}
