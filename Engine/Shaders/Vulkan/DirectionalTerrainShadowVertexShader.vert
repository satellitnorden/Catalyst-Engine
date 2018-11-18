//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec4 vertexLayerWeights;
layout (location = 3) in vec2 vertexTextureCoordinate;

void main()
{	
    //Set the position.
    gl_Position = viewMatrix * vec4(vertexPosition, 1.0f);
}