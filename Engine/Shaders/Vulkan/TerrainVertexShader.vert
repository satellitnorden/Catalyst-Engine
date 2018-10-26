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
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out vec4 fragmentLayerWeights;

void main()
{	
	//Pass information to the fragment shader.
	fragmentNormal = vertexNormal;
    fragmentLayerWeights = vertexLayerWeights;

    //Set the position.
    gl_Position = viewMatrix * vec4(vertexPosition, 1.0f);
}