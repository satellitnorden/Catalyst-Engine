#version 460

layout (location = 0) out vec2 OutTextureCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutTextureCoordinate.x = (x + 1.0f) * 0.5f;
    OutTextureCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}