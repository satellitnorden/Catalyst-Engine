//Includes.
#define CATALYST_LIGHTING_DATA_SET_INDEX (2)
#include "CatalystLightingData.glsl"
#include "CatalystMaterialCore.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRayTracingData.glsl"
#include "CatalystTerrainUtilities.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystTerrain.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

/*
*	Surface properties struct definition.
*/
struct SurfaceProperties
{
	vec3 albedo;
	vec3 shading_normal;
	vec4 material_properties;
};

//In parameters.
layout(location = 0) rayPayloadInNV PathTracingRayPayload path_tracing_ray_payload;
layout(location = 1) rayPayloadNV float visibility;
hitAttributeNV vec3 hit_attribute;

/*
*	Calculates static model surface properties.
*/
SurfaceProperties CalculateStaticModelSurfaceProperties(vec3 hit_position)
{
	//Unpack the vertices making up the triangle.
#if defined(STATIC_MODELS)
	Vertex vertex_1 = UnpackStaticModelVertex(gl_InstanceCustomIndexNV, STATIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].STATIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3]);
	Vertex vertex_2 = UnpackStaticModelVertex(gl_InstanceCustomIndexNV, STATIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].STATIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3 + 1]);
	Vertex vertex_3 = UnpackStaticModelVertex(gl_InstanceCustomIndexNV, STATIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].STATIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3 + 2]);
#elif defined(DYNAMIC_MODELS)
	Vertex vertex_1 = UnpackDynamicModelVertex(gl_InstanceCustomIndexNV, DYNAMIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].DYNAMIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3]);
	Vertex vertex_2 = UnpackDynamicModelVertex(gl_InstanceCustomIndexNV, DYNAMIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].DYNAMIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3 + 1]);
	Vertex vertex_3 = UnpackDynamicModelVertex(gl_InstanceCustomIndexNV, DYNAMIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].DYNAMIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3 + 2]);
	#endif

	//Calculate the final vertex using the barycentric coordinates.
	vec3 barycentric_coordinates = vec3(1.0f - hit_attribute.x - hit_attribute.y, hit_attribute.x, hit_attribute.y);

	Vertex final_vertex;

	final_vertex.position = vertex_1.position * barycentric_coordinates.x + vertex_2.position * barycentric_coordinates.y + vertex_3.position * barycentric_coordinates.z;
	final_vertex.normal = vertex_1.normal * barycentric_coordinates.x + vertex_2.normal * barycentric_coordinates.y + vertex_3.normal * barycentric_coordinates.z;
	final_vertex.tangent = vertex_1.tangent * barycentric_coordinates.x + vertex_2.tangent * barycentric_coordinates.y + vertex_3.tangent * barycentric_coordinates.z;
	final_vertex.texture_coordinate = vertex_1.texture_coordinate * barycentric_coordinates.x + vertex_2.texture_coordinate * barycentric_coordinates.y + vertex_3.texture_coordinate * barycentric_coordinates.z;

	//Transform the final vertex into world space.
	final_vertex.position = gl_ObjectToWorldNV * vec4(final_vertex.position, 1.0f);
	final_vertex.normal = normalize(gl_ObjectToWorldNV * vec4(final_vertex.normal, 0.0f));
	final_vertex.tangent = normalize(gl_ObjectToWorldNV * vec4(final_vertex.tangent, 0.0f));

	//Retrieve the material.
#if defined(STATIC_MODELS)
	Material material = GLOBAL_MATERIALS[UnpackStaticModelMaterialindex(gl_InstanceCustomIndexNV)];
#elif defined(DYNAMIC_MODELS)
	Material material = GLOBAL_MATERIALS[UnpackDynamicModelMaterialindex(gl_InstanceCustomIndexNV)];
#endif

	//Evaluate the material.
	vec4 albedo_thickness;
	vec4 normal_map_displacement;
	vec4 material_properties;
	vec4 opacity;

	EvaluateMaterial(material, final_vertex.texture_coordinate, albedo_thickness, normal_map_displacement, material_properties, opacity);

	//Calculate the shading normal.
	vec3 shading_normal = normal_map_displacement.xyz * 2.0f - 1.0f;
	shading_normal = mat3(final_vertex.tangent, cross(final_vertex.normal, final_vertex.tangent), final_vertex.normal) * shading_normal;
	shading_normal = normalize(shading_normal);

	//Fill in the surface properties.
	SurfaceProperties surface_properties;

	surface_properties.albedo = albedo_thickness.rgb;
	surface_properties.shading_normal = shading_normal;
	surface_properties.material_properties = vec4(material_properties[0], material_properties[1], material_properties[2], material_properties[3] * material._EmissiveMultiplier);

	return surface_properties;
}

/*
*	Calculates luminance lighting.
*/
vec3 CalculateLuminanceLighting(SurfaceProperties surface_properties)
{
	return surface_properties.albedo * surface_properties.material_properties[3];
}

/*
*	Calculates direct lighting.
*/
vec3 CalculateDirectLighting(vec3 hit_position, SurfaceProperties surface_properties)
{
	//Calculate the direct lighting.
	vec3 direct_lighting = vec3(0.0f);

	//Calculate all lights.
	for (int i = 0; i < LIGHT_HEADER_DATA._NumberOfLights; ++i)
	{
		Light light = UnpackLight(i);

		switch (light.light_type)
		{
			case LIGHT_TYPE_DIRECTIONAL:
			{
				//Alter the direction a bit to simulare soft shadows.
				vec3 light_direction = normalize(light.position_or_direction + vec3(path_tracing_ray_payload.random_noise.xyz * 2.0f - 1.0f) * 0.01f);

				//Trace the visibility.
				bool hit_anything = false;

				if (TEST_BIT(light.light_properties, LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT))
				{
					visibility = 0.0f;

					traceNV(TOP_LEVEL_ACCELERATION_STRUCTURE, 															//topLevel
							gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
							0xff, 																						//cullMask
							0, 																							//sbtRecordOffset
							0, 																							//sbtRecordStride
							1, 																							//missIndex
							hit_position, 																				//origin
							CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
							-light_direction,																			//direction
							VIEW_DISTANCE,																				//Tmax
							1 																							//payload
							);

					hit_anything = visibility < 1.0f;
				}

				if (!hit_anything)
				{
					direct_lighting += CalculateLighting(	-gl_WorldRayDirectionNV,
															surface_properties.albedo,
															surface_properties.shading_normal,
															surface_properties.material_properties[0],
															surface_properties.material_properties[1],
															surface_properties.material_properties[2],
															1.0f,
															light_direction,
															light.color * light.intensity);
				}
				

				break;
			}

			case LIGHT_TYPE_POINT:
			{
				//Calculate the light direction.
				vec3 light_direction = hit_position - (light.position_or_direction + light.size * vec3(path_tracing_ray_payload.random_noise.xyz * 2.0f - 1.0f));

				//Calculate the distance to the light.
				float distance_to_light = LengthSquared3(light_direction);

				//Only calculate lighting if the the world position is within the light's radius.
				if (distance_to_light < light.radius * light.radius)
				{
					//Perform the square root.
					distance_to_light = sqrt(distance_to_light);

					//Normalize the light direction.
					float distance_to_light_reciprocal = 1.0f / distance_to_light;
					light_direction *= distance_to_light_reciprocal;

					//Trace the visibility.
					bool hit_anything = false;

					if (TEST_BIT(light.light_properties, LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT))
					{
						visibility = 0.0f;

						traceNV
						(
							TOP_LEVEL_ACCELERATION_STRUCTURE, 															//topLevel
							gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
							0xff, 																						//cullMask
							0, 																							//sbtRecordOffset
							0, 																							//sbtRecordStride
							1, 																							//missIndex
							hit_position, 																				//origin
							CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
							-light_direction,																			//direction
							distance_to_light,																			//Tmax
							1 																							//payload
						);

						hit_anything = visibility < 1.0f;
					}

					if (!hit_anything)
					{
						//Calculate the attenuation.
						float attenuation = CalculateAttenuation(distance_to_light, light.radius);

						direct_lighting += CalculateLighting(	-gl_WorldRayDirectionNV,
																surface_properties.albedo,
																surface_properties.shading_normal,
																surface_properties.material_properties[0],
																surface_properties.material_properties[1],
																surface_properties.material_properties[2],
																1.0f,
																light_direction,
																light.color * light.intensity) * attenuation;
					}
				}

				break;
			}

			case LIGHT_TYPE_BOX:
			{
				//Calculate the light position.
				vec3 light_position = vec3
				(
					clamp(hit_position.x, light.position_or_direction.x, light.transform_data_2.x),
					clamp(hit_position.y, light.position_or_direction.y, light.transform_data_2.y),
					clamp(hit_position.z, light.position_or_direction.z, light.transform_data_2.z)
				);

				//Calculate the light direction.
				vec3 light_direction = hit_position - (light_position + light.size * vec3(path_tracing_ray_payload.random_noise.xyz * 2.0f - 1.0f));

				//Calculate the distance to the light.
				float distance_to_light = LengthSquared3(light_direction);

				//Only calculate lighting if the the world position is within the light's radius.
				if (distance_to_light < light.radius * light.radius)
				{
					//Perform the square root.
					distance_to_light = sqrt(distance_to_light);

					//Normalize the light direction.
					float distance_to_light_reciprocal = 1.0f / distance_to_light;
					light_direction *= distance_to_light_reciprocal;

					//Trace the visibility.
					visibility = 0.0f;

					traceNV(TOP_LEVEL_ACCELERATION_STRUCTURE, 															//topLevel
							gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
							0xff, 																						//cullMask
							0, 																							//sbtRecordOffset
							0, 																							//sbtRecordStride
							1, 																							//missIndex
							hit_position, 																				//origin
							CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
							-light_direction,																			//direction
							distance_to_light,																			//Tmax
							1 																							//payload
							);

					if (visibility == 1.0f)
					{
						//Calculate the attenuation.
						float attenuation = CalculateAttenuation(distance_to_light, light.radius);

						direct_lighting += CalculateLighting(	-gl_WorldRayDirectionNV,
																surface_properties.albedo,
																surface_properties.shading_normal,
																surface_properties.material_properties[0],
																surface_properties.material_properties[1],
																surface_properties.material_properties[2],
																1.0f,
																light_direction,
																light.color * light.intensity) * attenuation;
					}
				}

				break;
			}
		}
	}

	return direct_lighting;
}

/*
*	Calculates indirect lighting.
*/
vec3 CalculateIndirectLighting(uint current_recursion_depth, vec3 hit_position, SurfaceProperties surface_properties)
{
	//Calculate the indirect lighting direction.
	vec3 indirect_lighting_direction = CalculateGramSchmidtRotationMatrix(surface_properties.shading_normal, path_tracing_ray_payload.random_noise.xyz * 2.0f - 1.0f) * path_tracing_ray_payload.random_hemisphere_sample.xyz;

	//Flip the direction, if needed.
	indirect_lighting_direction = dot(indirect_lighting_direction, surface_properties.shading_normal) >= 0.0f ? indirect_lighting_direction : -indirect_lighting_direction;

	if (current_recursion_depth < 2)
	{
		//Reset the payload.
		path_tracing_ray_payload.current_recursion_depth = current_recursion_depth + 1;
		path_tracing_ray_payload.hit_distance = VIEW_DISTANCE;

		//Fire the ray!
		traceNV(
				TOP_LEVEL_ACCELERATION_STRUCTURE,			//topLevel
				gl_RayFlagsOpaqueNV, 						//rayFlags
				0xff, 										//cullMask
				0, 											//sbtRecordOffset
				0, 											//sbtRecordStride
				0, 											//missIndex
				hit_position, 								//origin
				CATALYST_RAY_TRACING_T_MINIMUM, 			//Tmin
				indirect_lighting_direction, 				//direction
				VIEW_DISTANCE, 								//Tmax
				0 											//payload
				);

		return CalculateLighting(	-gl_WorldRayDirectionNV,
									surface_properties.albedo,
									surface_properties.shading_normal,
									surface_properties.material_properties[0],
									surface_properties.material_properties[1],
									surface_properties.material_properties[2],
									1.0f,
									-indirect_lighting_direction,
									path_tracing_ray_payload.radiance);
	}
	
	else
	{
		return vec3(0.0f);
	}
}

void CatalystShaderMain()
{
	//Remember the current recursion depth.
	uint current_recursion_depth = path_tracing_ray_payload.current_recursion_depth;

	//Calculate the hit position.
	vec3 hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV;

	//Gather the surface properties.
	SurfaceProperties surface_properties = CalculateStaticModelSurfaceProperties(hit_position);

	//Calculate the lighting.
	vec3 lighting = vec3(0.0f);

	//Calculate the luminance lighting.
	lighting += CalculateLuminanceLighting(surface_properties);

	//Calculate the direct lighting.
	lighting += CalculateDirectLighting(hit_position, surface_properties);

	//Calculate the indirect lighting.
	lighting += CalculateIndirectLighting(current_recursion_depth, hit_position, surface_properties);

	//Calculate the volumetric lighting.
	vec3 volumetric_lighting = vec3(0.0f);
	float volumetric_lighting_opacity = 0.0f;

	{
		//Calculate the volumetric hit distance.
		float volumetric_hit_distance = min(gl_HitTNV, VOLUMETRIC_LIGHTING_DISTANCE);

		//Sample the noise.
		vec4 noise_sample = SampleBlueNoiseTexture(uvec2(gl_LaunchIDNV.xy), 0);

		//Calculate the volumetric particle hit position.
		vec3 volumetric_particle_hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * volumetric_hit_distance * noise_sample[0];

		//Calculate all lights.
		for (int light_index = 0; light_index < LIGHT_HEADER_DATA._NumberOfLights; ++light_index)
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

		//Add the ambient lighting.
		volumetric_lighting += CalculateVolumetricAmbientLighting();

		//Calculate the volumetric lighting opacity.
		volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(volumetric_hit_distance, VOLUMETRIC_LIGHTING_DISTANCE, hit_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, CAMERA_WORLD_POSITION.y);
	}

	//Write to the ray payload.
	path_tracing_ray_payload.radiance 				= mix(lighting, volumetric_lighting, volumetric_lighting_opacity);
	path_tracing_ray_payload.albedo 				= surface_properties.albedo;
	path_tracing_ray_payload.shading_normal 		= surface_properties.shading_normal;
	path_tracing_ray_payload.hit_distance 			= gl_HitTNV;
	path_tracing_ray_payload.material_properties 	= surface_properties.material_properties;
}