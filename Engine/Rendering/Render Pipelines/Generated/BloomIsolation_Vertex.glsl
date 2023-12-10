#version 460

layout (std140, set = 0, binding = 0) uniform PostProcessing
{
	layout (offset = 0) float BLOOM_THRESHOLD;
	layout (offset = 4) float BLOOM_INTENSITY;
};

#define FLOAT32_EPSILON (1.192092896e-07F)

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}