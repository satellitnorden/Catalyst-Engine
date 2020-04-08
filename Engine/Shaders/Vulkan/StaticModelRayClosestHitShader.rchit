//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRayTracingData.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystTerrainUtilities.glsl"
#include "..\..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\..\Include\Rendering\Native\Shader\CatalystTerrain.h"

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
layout(location = 1) rayPayloadInNV float visibility;
hitAttributeNV vec3 hit_attribute;

/*
*	Calculates static model surface properties.
*/
SurfaceProperties CalculateStaticModelSurfaceProperties(vec3 hit_position)
{
	//Unpack the vertices making up the triangle.
	Vertex vertex_1 = UnpackStaticModelVertex(gl_InstanceCustomIndexNV, STATIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].STATIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3]);
	Vertex vertex_2 = UnpackStaticModelVertex(gl_InstanceCustomIndexNV, STATIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].STATIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3 + 1]);
	Vertex vertex_3 = UnpackStaticModelVertex(gl_InstanceCustomIndexNV, STATIC_MODEL_INDEX_BUFFERS[gl_InstanceCustomIndexNV].STATIC_MODEL_INDEX_DATA[gl_PrimitiveID * 3 + 2]);

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
	Material material = GLOBAL_MATERIALS[UnpackStaticModelMaterialindex(gl_InstanceCustomIndexNV)];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, final_vertex.texture_coordinate);

	//Sample the material properties.
	vec4 material_properties = RetrieveMaterialProperties(material, final_vertex.texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal;

	if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
	{
		shading_normal = final_vertex.normal;
	}

	else
	{
		//Sample the normal map.
		vec3 normal_map = texture(sampler2D(GLOBAL_TEXTURES[material.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), final_vertex.texture_coordinate).xyz;
		shading_normal = normal_map * 2.0f - 1.0f;
		shading_normal = mat3(final_vertex.tangent, cross(final_vertex.normal, final_vertex.tangent), final_vertex.normal) * shading_normal;
		shading_normal = normalize(shading_normal);
	}

	//Gather the surface properties.
	SurfaceProperties surface_properties;

	//Fill in the surface properties.
	surface_properties.albedo = albedo;
	surface_properties.shading_normal = shading_normal;
	surface_properties.material_properties = material_properties;

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
	for (int i = 0; i < NUMBER_OF_LIGHTS; ++i)
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

				visibility = 0.0f;

				traceNV(
						TERRAIN_TOP_LEVEL_ACCELERATION_STRUCTURE, 													//topLevel
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

				if (!hit_anything)
				{
					visibility = 0.0f;

					traceNV(
							STATIC_TOP_LEVEL_ACCELERATION_STRUCTURE, 													//topLevel
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

					if (!hit_anything)
					{
						visibility = 0.0f;

						traceNV(
								DYNAMIC_TOP_LEVEL_ACCELERATION_STRUCTURE, 													//topLevel
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
				//TODO

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

	//Calculate the reflection direction.
	vec3 reflection_direction = reflect(gl_WorldRayDirectionNV, surface_properties.shading_normal);

	//Blend the random hemisphere direction and the reflection direction based on the material properties.
	indirect_lighting_direction = normalize(mix(reflection_direction, indirect_lighting_direction, surface_properties.material_properties[0]));

	if (current_recursion_depth < 1)
	{
		//Fire the ray(s)!
		bool has_hit = false;
		float closest_hit_distance = VIEW_DISTANCE;

		//Terrain.
		{
			//Reset the payload.
			path_tracing_ray_payload.current_recursion_depth = current_recursion_depth + 1;
			path_tracing_ray_payload.hit_distance = VIEW_DISTANCE;

			//Fire the ray!
			traceNV(
					TERRAIN_TOP_LEVEL_ACCELERATION_STRUCTURE,	//topLevel
					gl_RayFlagsOpaqueNV, 						//rayFlags
					0xff, 										//cullMask
					0, 											//sbtRecordOffset
					0, 											//sbtRecordStride
					0, 											//missIndex
					hit_position, 								//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 			//Tmin
					indirect_lighting_direction, 				//direction
					closest_hit_distance, 						//Tmax
					0 											//payload
					);

			closest_hit_distance = min(closest_hit_distance, path_tracing_ray_payload.hit_distance);

			//Keep track of whether or not there was a hit.
			if (path_tracing_ray_payload.hit_distance < VIEW_DISTANCE)
			{
				has_hit = true;
			}
		}

		//Static models.
		{
			//Reset the payload.
			path_tracing_ray_payload.current_recursion_depth = current_recursion_depth + 1;
			path_tracing_ray_payload.hit_distance = VIEW_DISTANCE;

			//Fire the ray!
			traceNV(
					STATIC_TOP_LEVEL_ACCELERATION_STRUCTURE,	//topLevel
					gl_RayFlagsOpaqueNV, 						//rayFlags
					0xff, 										//cullMask
					0, 											//sbtRecordOffset
					0, 											//sbtRecordStride
					0, 											//missIndex
					hit_position, 								//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 			//Tmin
					indirect_lighting_direction, 				//direction
					closest_hit_distance, 						//Tmax
					0 											//payload
					);

			closest_hit_distance = min(closest_hit_distance, path_tracing_ray_payload.hit_distance);

			//Keep track of whether or not there was a hit.
			if (path_tracing_ray_payload.hit_distance < VIEW_DISTANCE)
			{
				has_hit = true;
			}
		}

		//Was there a hit?
		if (has_hit)
		{
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
			return CalculateLighting(	-gl_WorldRayDirectionNV,
										surface_properties.albedo,
										surface_properties.shading_normal,
										surface_properties.material_properties[0],
										surface_properties.material_properties[1],
										surface_properties.material_properties[2],
										1.0f,
										-indirect_lighting_direction,
										texture(SKY_TEXTURES[0], indirect_lighting_direction).rgb);
		}
	}
	
	else
	{
		return CalculateLighting(	-gl_WorldRayDirectionNV,
									surface_properties.albedo,
									surface_properties.shading_normal,
									surface_properties.material_properties[0],
									surface_properties.material_properties[1],
									surface_properties.material_properties[2],
									1.0f,
									-indirect_lighting_direction,
									texture(SKY_TEXTURES[0], indirect_lighting_direction).rgb);
	}
}

void main()
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

	//Write to the ray payload.
	path_tracing_ray_payload.radiance 				= lighting;
	path_tracing_ray_payload.albedo 				= surface_properties.albedo;
	path_tracing_ray_payload.shading_normal 		= surface_properties.shading_normal;
	path_tracing_ray_payload.hit_distance 			= gl_HitTNV;
	path_tracing_ray_payload.material_properties 	= surface_properties.material_properties;
}