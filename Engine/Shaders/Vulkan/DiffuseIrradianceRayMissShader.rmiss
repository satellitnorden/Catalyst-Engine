//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Descriptor set data.
layout (set = 3, binding = 2) uniform samplerCube environmentTexture;

//In parameters.
layout(location = 0) rayPayloadInNV DiffuseIrradianceRayPayload rayPayload;

void main()
{
	//Write to the ray payload.
	rayPayload.directLighting = texture(environmentTexture, gl_WorldRayDirectionNV).rgb;
}