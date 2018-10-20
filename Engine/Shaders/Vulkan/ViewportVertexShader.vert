//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//The viewport vertices.
vec3 viewportVertices[4] = vec3[]
(
	vec3(-1.0f, -1.0f, 0.0f),
	vec3(-1.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 0.0f),
 	vec3(1.0f, -1.0f, 0.0f)
);

//The viewport texture coordinates.
vec2 viewportTextureCoordinates[4] = vec2[]
(
	vec2(0.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 1.0f),
	vec2(1.0f, 0.0f)
);

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{
	//Set the fragment texture coordinate.
    fragmentTextureCoordinate = viewportTextureCoordinates[gl_VertexIndex];

    //Set the vertex position.
    gl_Position = vec4(viewportVertices[gl_VertexIndex], 1.0f);
} 