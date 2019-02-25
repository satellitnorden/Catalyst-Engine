//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 color;
    layout (offset = 16) vec3 worldPosition;
    layout (offset = 32) float attenuationDistance;
    layout (offset = 36) float intensity;
};

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
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