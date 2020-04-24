//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void CatalystShaderMain()
{
	//Calculate the viewport coordinates.
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);

    //Calculate the texture coordinate.
    fragmentTextureCoordinate.x = (x + 1.0f) * 0.5f;
    fragmentTextureCoordinate.y = (y + 1.0f) * 0.5f;
    
    //Set the position.
    gl_Position = vec4(x, y, 0.0f, 1.0f);
} 