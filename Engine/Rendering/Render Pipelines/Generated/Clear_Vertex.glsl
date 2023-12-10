#version 460

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
	gl_Position = vec4(x,y,0.0f,1.0f);
}