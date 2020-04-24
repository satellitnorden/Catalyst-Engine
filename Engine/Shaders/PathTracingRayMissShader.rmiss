//Includes.
#include "CatalystRayTracingCore.glsl"

//In parameters.
layout(location = 0) rayPayloadInNV PathTracingRayPayload path_tracing_ray_payload;

void CatalystShaderMain()
{
	//Write to the ray payload.
	path_tracing_ray_payload.radiance 				= vec3(0.0f, 0.0f, 0.0f);
	path_tracing_ray_payload.albedo 				= vec3(0.0f, 0.0f, 0.0f);
	path_tracing_ray_payload.shading_normal 		= vec3(0.0f, 1.0f, 0.0f);
	path_tracing_ray_payload.hit_distance 			= VIEW_DISTANCE;
	path_tracing_ray_payload.material_properties 	= vec4(0.0f, 0.0f, 0.0f, 0.0f);
}