//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Descriptor set data.
layout (set = 1, binding = 2) uniform samplerCube environmentTexture;

//In parameters.
layout(location = 0) rayPayloadInNV vec4 rayPayload;

void main()
{
    rayPayload.rgb = texture(environmentTexture, gl_WorldRayDirectionNV).rgb;
}