//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//In parameters.
layout(location = 0) rayPayloadInNV DiffuseIrradianceRayPayload rayPayload;

void main()
{
	//Write to the ray payload.
	rayPayload.directLighting = vec3(0.0f);
}