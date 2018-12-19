//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Model uniform buffer.
layout (push_constant) uniform ModelData
{
    layout (offset = 0) mat4 modelMatrix;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;

void main()
{
	//Calculate the fragment normal.
	fragmentNormal = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0f)));

	gl_Position = directionalLightViewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}