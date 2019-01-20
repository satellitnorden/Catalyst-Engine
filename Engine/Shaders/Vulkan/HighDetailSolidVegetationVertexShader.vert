//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;
layout (location = 4) in mat4 vertexTransformationMatrix;

//Out parameters.
layout (location = 0) out mat3 fragmentTangentSpaceMatrix;
layout (location = 3) out vec2 fragmentTextureCoordinate;


void main()
{
    //Pass along the texture coordinate to the fragment shader.
    fragmentTextureCoordinate = vertexTextureCoordinate;

    //Calculate the tangent space matrix.
    vec3 tangent = normalize(vec3(vertexTransformationMatrix * vec4(vertexTangent, 0.0f)));
    vec3 bitangent = normalize(vec3(vertexTransformationMatrix * vec4(cross(vertexNormal, vertexTangent), 0.0f)));
    vec3 normal = normalize(vec3(vertexTransformationMatrix * vec4(vertexNormal, 0.0f)));

    fragmentTangentSpaceMatrix = mat3(tangent, bitangent, normal);

    //Write the position.
    gl_Position = viewMatrix * vertexTransformationMatrix * vec4(vertexPosition, 1.0);
}