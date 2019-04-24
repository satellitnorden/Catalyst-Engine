//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Descriptor set data.
layout (set = 1, binding = 5) uniform samplerCube environmentTexture;

//In parameters.
layout(location = 0) rayPayloadInNV RayPayload rayPayload;

void main()
{
    rayPayload.radiance = texture(environmentTexture, gl_WorldRayDirectionNV).rgb;
}