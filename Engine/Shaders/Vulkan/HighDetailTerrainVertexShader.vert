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

//Out parameters.
layout (location = 0) out vec3 tessellationControlPosition;
layout (location = 1) out vec3 tessellationControlNormal;
layout (location = 2) out vec4 tessellationControlLayerWeights;
layout (location = 3) out vec2 tessellationControlTextureCoordinate;

void main()
{	
	//Pass information to the tessellation control shader.
    tessellationControlPosition = vertexPosition;
	tessellationControlNormal = vertexNormal;
    tessellationControlLayerWeights = vertexLayerWeights;
    tessellationControlTextureCoordinate = vec2(vertexPosition.x, vertexPosition.z);
}