//Includes.
#define CATALYST_LIGHTING_DATA_SET_INDEX (2)
#include "CatalystLightingData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRayTracingData.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//In parameters.
layout(location = 0) rayPayloadInNV PathTracingRayPayload path_tracing_ray_payload;
layout(location = 1) rayPayloadNV float visibility;

void CatalystShaderMain()
{
	//Calculate the hit position.
	vec3 hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * VIEW_DISTANCE;

	//Calculate the volumetric lighting.
	vec3 volumetric_lighting = vec3(0.0f);
	float volumetric_lighting_opacity = 0.0f;

	{
		//Add the ambient lighting.
		volumetric_lighting += CalculateVolumetricAmbientLighting();
		
		//Calculate the volumetric hit distance.
		float volumetric_hit_distance = min(VIEW_DISTANCE, VOLUMETRIC_LIGHTING_DISTANCE);

		//Sample the noise.
		vec4 noise_sample = SampleBlueNoiseTexture(uvec2(gl_LaunchIDNV.xy), 0);

		//Calculate the volumetric particle hit position.
		vec3 volumetric_particle_hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * volumetric_hit_distance * noise_sample[0];

		//Calculate all lights.
		for (int light_index = 0; light_index < NUMBER_OF_LIGHTS; ++light_index)
		{
			Light light = UnpackLight(light_index);

			switch (light.light_type)
			{
				case LIGHT_TYPE_DIRECTIONAL:
				{
					//Trace the visibility.
					visibility = 0.0f;

					traceNV(TOP_LEVEL_ACCELERATION_STRUCTURE, 															//topLevel
							gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
							0xff, 																						//cullMask
							0, 																							//sbtRecordOffset
							0, 																							//sbtRecordStride
							1, 																							//missIndex
							volumetric_particle_hit_position, 															//origin
							CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
							-light.position_or_direction,																//direction
							CATALYST_RAY_TRACING_T_MAXIMUM,																//Tmax
							1 																							//payload
							);

					volumetric_lighting += light.color * light.intensity * VOLUMETRIC_LIGHTING_BASE_COLOR * VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER * visibility;

					break;
				}

				case LIGHT_TYPE_POINT:
				{
					break;
				}
			}
		}

		//Calculate the volumetric lighting opacity.
		volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(volumetric_hit_distance, VOLUMETRIC_LIGHTING_DISTANCE, hit_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, gl_WorldRayOriginNV.y);
	}

	//Write to the ray payload.
	path_tracing_ray_payload.radiance 				= mix(SampleSky(gl_WorldRayDirectionNV, 0.0f), volumetric_lighting, volumetric_lighting_opacity);
	path_tracing_ray_payload.albedo 				= vec3(0.0f, 0.0f, 0.0f);
	path_tracing_ray_payload.shading_normal 		= -gl_WorldRayDirectionNV;
	path_tracing_ray_payload.hit_distance 			= VIEW_DISTANCE;
	path_tracing_ray_payload.material_properties 	= vec4(0.0f, 0.0f, 0.0f, 0.0f);
}