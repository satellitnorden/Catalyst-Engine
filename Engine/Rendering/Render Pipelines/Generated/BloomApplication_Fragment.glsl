#version 460

layout (location = 0) in vec2 InTextureCoordinate;

layout (set = 0, binding = 0) uniform sampler2D Scene;

layout (location = 0) out vec4 INTERMEDIATE_RGBA_FLOAT32_1;

void main()
{
    vec4 scene_texture_sample = texture(Scene, InTextureCoordinate);
	INTERMEDIATE_RGBA_FLOAT32_1 = scene_texture_sample;
}