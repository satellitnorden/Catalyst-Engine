#version 460

layout (std140, set = 0, binding = 0) uniform PostProcessing
{
	layout (offset = 0) float BLOOM_THRESHOLD;
	layout (offset = 4) float BLOOM_INTENSITY;
};

#define FLOAT32_EPSILON (1.192092896e-07F)

layout (location = 0) in vec2 InScreenCoordinate;

layout (set = 0, binding = 1) uniform sampler2D INTERMEDIATE_RGBA_FLOAT32_1;

layout (location = 0) out vec4 Scene;

void main()
{
    vec4 scene = texture(INTERMEDIATE_RGBA_FLOAT32_1, InScreenCoordinate);
    float brightness = max(scene.r, max(scene.g, scene.b));
    float contribution = max(brightness - BLOOM_THRESHOLD, 0.0f);
    contribution /= max(brightness, FLOAT32_EPSILON);
    contribution *= 0.5f;
	Scene = vec4(scene*contribution);
}