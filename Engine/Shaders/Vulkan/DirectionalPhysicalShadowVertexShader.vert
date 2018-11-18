//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//The cube map vertices.
vec3 cubeMapVertices[36] = vec3[]
(
  vec3(-1.0f, 1.0f, -1.0f),
  vec3(-1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, 1.0f, -1.0f),
  vec3(-1.0f, 1.0f, -1.0f),

  vec3(-1.0f, -1.0f, 1.0f),
  vec3(-1.0f, -1.0f, -1.0f),
  vec3(-1.0f, 1.0f, -1.0f),
  vec3(-1.0f, 1.0f, -1.0f),
  vec3(-1.0f, 1.0f, 1.0f),
  vec3(-1.0f, -1.0f, 1.0f),

  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),

  vec3(-1.0f, -1.0f, 1.0f),
  vec3(-1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, -1.0f, 1.0f),
  vec3(-1.0f, -1.0f, 1.0f),

  vec3(-1.0f, 1.0f, -1.0f),
  vec3(1.0f, 1.0f, -1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(-1.0f, 1.0f, 1.0f),
  vec3(-1.0f, 1.0f, -1.0f),

  vec3(-1.0f, -1.0f, -1.0f),
  vec3(-1.0f, -1.0f, 1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(-1.0f, -1.0f, 1.0f),
  vec3(1.0f, -1.0f, 1.0f)
);

//Model uniform buffer.
layout (push_constant) uniform ModelData
{
    mat4 modelMatrix;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;

void main()
{
  gl_Position = directionalLightViewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}