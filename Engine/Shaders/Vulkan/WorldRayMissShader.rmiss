//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Descriptor set data.
layout (set = 1, binding = 8) uniform samplerCube environmentTexture;

//In parameters.
layout(location = 0) rayPayloadInNV PrimaryRayPayload rayPayload;

void main()
{
	//Write to the ray payload.
	rayPayload.diffuseIrradiance = rayPayload.specularIrradiance = rayPayload.radiance = texture(environmentTexture, gl_WorldRayDirectionNV).rgb;
	rayPayload.geometryNormal = rayPayload.shadingNormal = vec3(0.0f, 1.0f, 0.0f);
	rayPayload.depth = CATALYST_RAY_TRACING_T_MAXIMUM;
	rayPayload.roughness = 1.0f;
	rayPayload.metallic = 0.0f;
	rayPayload.ambientOcclusion = 1.0f;
	rayPayload.emissive = 0.0f;
	rayPayload.instanceID = 0;
	rayPayload.primitiveID = 0;
	rayPayload.barycentricCoordinates = vec2(0.0f);
}