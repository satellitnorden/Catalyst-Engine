//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Descriptor set data.
layout (set = 1, binding = 10) uniform samplerCube environmentTexture;

//In parameters.
layout(location = 0) rayPayloadInNV PrimaryRayPayload rayPayload;

void main()
{
	//Write to the ray payload.
	rayPayload.directLighting = texture(environmentTexture, gl_WorldRayDirectionNV).rgb;
}