#version 460

layout (location = 0) in vec2 InTextureCoordinate;

layout (set = 0, binding = 0) uniform sampler2D Scene;

layout (location = 0) out vec4 Screen;

void main()
{
    vec4 scene_texture_sample = texture(Scene, InTextureCoordinate);
	Screen = scene_texture_sample;
}