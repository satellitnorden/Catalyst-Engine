//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
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
layout (location = 0) out mat3 fragmentTangentSpaceMatrix;
layout (location = 4) out vec3 fragmentWorldPosition;
layout (location = 5) out vec2 fragmentTextureCoordinate;

void main()
{
	fragmentWorldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    
	vec3 tangent = normalize(vec3(modelMatrix * vec4(vertexTangent, 0.0f)));
	vec3 bitangent = normalize(vec3(modelMatrix * vec4(cross(vertexNormal, vertexTangent), 0.0f)));
	vec3 normal = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0f)));

	fragmentTangentSpaceMatrix = mat3(tangent, bitangent, normal);

	fragmentTextureCoordinate = vertexTextureCoordinate;

	gl_Position = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}