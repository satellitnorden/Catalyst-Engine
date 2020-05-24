//Out parameters.
layout (location = 0) in vec4 fragment_color;
layout (location = 1) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout(set = 1, binding = 0) uniform sampler2D element_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Write the fragment.
    fragment = fragment_color * texture(element_texture, fragment_texture_coordinate);
}