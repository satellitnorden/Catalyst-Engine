//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//In parameters.
layout(location = 0) rayPayloadInNV PathTracingRayPayload rayPayload;

void main()
{
	//Write to the ray payload.
	rayPayload.radiance = vec3(0.0f);
	rayPayload.albedo = vec3(0.0f);
	rayPayload.geometryNormal = vec3(0.0f, 1.0f, 0.0f);
	rayPayload.hitDistance = viewDistance;
	rayPayload.shadingNormal = vec3(0.0f, 1.0f, 0.0f);
	rayPayload.materialProperties = 0;
	rayPayload.roughness = 1.0f;
	rayPayload.metallic = 0.0f;
	rayPayload.ambientOcclusion = 1.0f;
	rayPayload.luminance = 0.0f;	
}