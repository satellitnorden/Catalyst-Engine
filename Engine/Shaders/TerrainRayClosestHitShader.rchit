//Includes.
#include "CatalystLightingData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRayTracingData.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystTerrainUtilities.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystTerrain.h"

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
*	Calculates terrain surface properties.
*/
SurfaceProperties CalculateTerrainSurfaceProperties(vec3 hit_position)
{
	//Gather the surface properties.
	SurfaceProperties surface_properties;

	//Calculate the terrain map texture coordinate.
	vec2 terrain_map_texture_coordinate = (hit_position.xz + (TERRAIN_MAP_RESOLUTION * 0.5f)) / TERRAIN_MAP_RESOLUTION;

	//Calculate the material texture coordinate.
	vec2 material_texture_coordinate = hit_position.xz * 0.25f;

	//Calculate the material.
	TerrainMaterial first_material 	= CalculateTerrainMaterial(terrain_map_texture_coordinate, material_texture_coordinate);
	TerrainMaterial second_material = CalculateTerrainMaterial(terrain_map_texture_coordinate + vec2(0.0f, 1.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);
	TerrainMaterial third_material 	= CalculateTerrainMaterial(terrain_map_texture_coordinate + vec2(1.0f, 0.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);
	TerrainMaterial fourth_material = CalculateTerrainMaterial(terrain_map_texture_coordinate + vec2(1.0f, 1.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);

	TerrainMaterial blend_1 = BlendTerrainMaterial(first_material, second_material, fract(terrain_map_texture_coordinate.y * TERRAIN_MAP_RESOLUTION));
	TerrainMaterial blend_2 = BlendTerrainMaterial(third_material, fourth_material, fract(terrain_map_texture_coordinate.y * TERRAIN_MAP_RESOLUTION));

	TerrainMaterial final_terrain_material = BlendTerrainMaterial(blend_1, blend_2, fract(terrain_map_texture_coordinate.x * TERRAIN_MAP_RESOLUTION));

	final_terrain_material.normal_map = normalize(final_terrain_material.normal_map);

	//Calculate the surrounding heights.
	#define OFFSET (1.0f / TERRAIN_MAP_RESOLUTION)

	float center_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate).x;
	float right_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate + vec2(OFFSET, 0.0f)).x;
	float up_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate + vec2(0.0f, OFFSET)).x;

	//Calculate the terrain tangent space matrix.
	mat3 terrain_tangent_space_matrix = CalculateTerrainTangentSpaceMatrix(center_height, right_height, up_height);

	//Fill in the surface properties.
	surface_properties.albedo = final_terrain_material.albedo;
	surface_properties.shading_normal = normalize(terrain_tangent_space_matrix * final_terrain_material.normal_map);
	surface_properties.material_properties = final_terrain_material.material_properties;

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
						TOP_LEVEL_ACCELERATION_STRUCTURE, 															//topLevel
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
				closest_hit_distance, 						//Tmax
				0 											//payload
				);

		closest_hit_distance = min(closest_hit_distance, path_tracing_ray_payload.hit_distance);

		//Keep track of whether or not there was a hit.
		if (path_tracing_ray_payload.hit_distance < VIEW_DISTANCE)
		{
			has_hit = true;
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
										texture(SKY_TEXTURE, indirect_lighting_direction).rgb * SKY_INTENSITY);
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
									texture(SKY_TEXTURE, indirect_lighting_direction).rgb * SKY_INTENSITY);
	}
}

void CatalystShaderMain()
{
	//Remember the current recursion depth.
	uint current_recursion_depth = path_tracing_ray_payload.current_recursion_depth;

	//Calculate the hit position.
	vec3 hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV;

	//Gather the surface properties.
	SurfaceProperties surface_properties = CalculateTerrainSurfaceProperties(hit_position);

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