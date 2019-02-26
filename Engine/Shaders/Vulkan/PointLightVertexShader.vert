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

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;

void main()
{
    //Write the position.
    gl_Position = viewMatrix * vec4(worldPosition + vertexPosition * attenuationDistance * 2.0f, 1.0);
} 