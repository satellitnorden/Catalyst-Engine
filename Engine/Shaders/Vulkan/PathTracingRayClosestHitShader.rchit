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

/*
*	Terrain material struct definition.
*/
struct TerrainMaterial
{
	vec3 albedo;
	vec3 normal_map;
	vec4 material_properties;
};

//In parameters.
layout(location = 0) rayPayloadInNV PathTracingRayPayload path_tracing_ray_payload;
layout(location = 1) rayPayloadInNV float visibility;
hitAttributeNV vec3 hit_attribute;

/*
*	Calculates the terrain material.
*/
TerrainMaterial CalculateTerrainMaterial(vec2 terrain_map_texture_coordinate, vec2 material_texture_coordinate)
{
	//Retrieve the 4 materials to blend between.
	vec4 index_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_INDEX_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate);

	Material material_1 = GLOBAL_MATERIALS[int(index_map[0] * 255.0f)];
	Material material_2 = GLOBAL_MATERIALS[int(index_map[1] * 255.0f)];
	Material material_3 = GLOBAL_MATERIALS[int(index_map[2] * 255.0f)];
	Material material_4 = GLOBAL_MATERIALS[int(index_map[3] * 255.0f)];

	//Retrieve the 4 displacement values.
	float displacement_1 = texture(sampler2D(GLOBAL_TEXTURES[material_1.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_2 = texture(sampler2D(GLOBAL_TEXTURES[material_2.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_3 = texture(sampler2D(GLOBAL_TEXTURES[material_3.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_4 = texture(sampler2D(GLOBAL_TEXTURES[material_4.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;

	//Retrieve the blend map.
	vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_BLEND_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate);

	//Alter the blend values based on the displacement values.
	#define STRENGTHEN_DISPLACEMENT(X) (X * X * X)

	blend_map[0] *= STRENGTHEN_DISPLACEMENT(displacement_1);
	blend_map[1] *= STRENGTHEN_DISPLACEMENT(displacement_2);
	blend_map[2] *= STRENGTHEN_DISPLACEMENT(displacement_3);
	blend_map[3] *= STRENGTHEN_DISPLACEMENT(displacement_4);

	//Renormalize the blend map.
	float inverse_sum = 1.0f / (blend_map[0] + blend_map[1] + blend_map[2] + blend_map[3]);

	blend_map[0] *= inverse_sum;
	blend_map[1] *= inverse_sum;
	blend_map[2] *= inverse_sum;
	blend_map[3] *= inverse_sum;

	//Blend the material.
	TerrainMaterial material;

	material.albedo = 	texture(sampler2D(GLOBAL_TEXTURES[material_1.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[0]
						+ texture(sampler2D(GLOBAL_TEXTURES[material_2.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[1]
						+ texture(sampler2D(GLOBAL_TEXTURES[material_3.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[2]
						+ texture(sampler2D(GLOBAL_TEXTURES[material_4.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[3];

	material.normal_map =	(texture(sampler2D(GLOBAL_TEXTURES[material_1.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[0]
							+ (texture(sampler2D(GLOBAL_TEXTURES[material_2.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[1]
							+ (texture(sampler2D(GLOBAL_TEXTURES[material_3.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[2]
							+ (texture(sampler2D(GLOBAL_TEXTURES[material_4.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[3];

	material.normal_map = normalize(material.normal_map);

	material.material_properties = 	texture(sampler2D(GLOBAL_TEXTURES[material_1.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[0]
									+ texture(sampler2D(GLOBAL_TEXTURES[material_2.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[1]
									+ texture(sampler2D(GLOBAL_TEXTURES[material_3.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[2]
									+ texture(sampler2D(GLOBAL_TEXTURES[material_4.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[3];

	return material;
}

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

	//Calculate the terrain material.
	TerrainMaterial terrain_material = CalculateTerrainMaterial(terrain_map_texture_coordinate, material_texture_coordinate);

	//Calculate the surrounding heights.
	#define OFFSET (1.0f / TERRAIN_MAP_RESOLUTION)

	float center_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate).x;
	float right_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate + vec2(OFFSET, 0.0f)).x;
	float up_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), terrain_map_texture_coordinate + vec2(0.0f, OFFSET)).x;

	//Calculate the terrain tangent space matrix.
	mat3 terrain_tangent_space_matrix = CalculateTerrainTangentSpaceMatrix(center_height, right_height, up_height);

	//Fill in the surface properties.
	surface_properties.albedo = terrain_material.albedo;
	surface_properties.shading_normal = normalize(terrain_tangent_space_matrix * terrain_material.normal_map);
	surface_properties.material_properties = terrain_material.material_properties;

	return surface_properties;
}

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
	return surface_properties.albedo * surface_properties.material_properties[3] * GLOBAL_MATERIALS[UnpackStaticModelMaterialindex(gl_InstanceCustomIndexNV)].luminance_multiplier;
}

/*
*	Calculates direct lighting.
*/
vec3 CalculateDirectLighting(vec3 hit_position, SurfaceProperties surface_properties)
{
	//Calculate the direct lighting.
	vec3 direct_lighting = vec3(0.0f);

	//Calculate all lights.
	for (int i = 0; i < numberOfLights; ++i)
	{
		Light light = UnpackLight(i);

		switch (light.type)
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
															light.luminance) * (1.0f - CLOUD_DENSITY);
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
			path_tracing_ray_payload.type = CATALYST_PATH_TRACING_TYPE_TERRAIN;
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
			path_tracing_ray_payload.type = CATALYST_PATH_TRACING_TYPE_STATIC_MODELS;
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
	SurfaceProperties surface_properties;

	switch (path_tracing_ray_payload.type)
	{
		case CATALYST_PATH_TRACING_TYPE_TERRAIN:
		{
			surface_properties = CalculateTerrainSurfaceProperties(hit_position);

			break;
		}

		case CATALYST_PATH_TRACING_TYPE_STATIC_MODELS:
		{
			surface_properties = CalculateStaticModelSurfaceProperties(hit_position);

			break;
		}
	}

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

	/*
	//Store the current recursion depth.
	int currentRecursionDepth = rayPayload.currentRecursionDepth;

	//Sample the noise texture.
	vec3 randomDirection = normalize(rayPayload.randomVector.xyz * 2.0f - 1.0f);
	float randomValue = rayPayload.randomVector.w;

	//Unpack the vertices making up the triangle.
	Vertex vertex1 = UnpackVertex(gl_InstanceCustomIndexNV, indexBuffers[gl_InstanceCustomIndexNV].indicesData[gl_PrimitiveID * 3]);
	Vertex vertex2 = UnpackVertex(gl_InstanceCustomIndexNV, indexBuffers[gl_InstanceCustomIndexNV].indicesData[gl_PrimitiveID * 3 + 1]);
	Vertex vertex3 = UnpackVertex(gl_InstanceCustomIndexNV, indexBuffers[gl_InstanceCustomIndexNV].indicesData[gl_PrimitiveID * 3 + 2]);

	//Calculate the final vertex using the barycentric coordinates.
	vec3 barycentricCoordinates = vec3(1.0f - hitAttribute.x - hitAttribute.y, hitAttribute.x, hitAttribute.y);

	Vertex finalVertex;

	finalVertex.position = vertex1.position * barycentricCoordinates.x + vertex2.position * barycentricCoordinates.y + vertex3.position * barycentricCoordinates.z;
	finalVertex.normal = vertex1.normal * barycentricCoordinates.x + vertex2.normal * barycentricCoordinates.y + vertex3.normal * barycentricCoordinates.z;
	finalVertex.tangent = vertex1.tangent * barycentricCoordinates.x + vertex2.tangent * barycentricCoordinates.y + vertex3.tangent * barycentricCoordinates.z;
	finalVertex.textureCoordinate = vertex1.textureCoordinate * barycentricCoordinates.x + vertex2.textureCoordinate * barycentricCoordinates.y + vertex3.textureCoordinate * barycentricCoordinates.z;

	//Transform the final vertex into world space.
	finalVertex.position = gl_ObjectToWorldNV * vec4(finalVertex.position, 1.0f);
	finalVertex.normal = gl_ObjectToWorldNV * vec4(finalVertex.normal, 0.0f);
	finalVertex.tangent = gl_ObjectToWorldNV * vec4(finalVertex.tangent, 0.0f);

	//Store the geometry normal.
	vec3 geometryNormal = finalVertex.normal;

	//Calculate the hit position.
	vec3 hitPosition = finalVertex.position + geometryNormal * 0.00001f;

	//Sample the albedo.
	vec3 albedo = pow(texture(sampler2D(GLOBAL_TEXTURES[modelMaterials[gl_InstanceCustomIndexNV].albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate).rgb, vec3(2.2f));

	//Sample the normal map.
	vec3 normalMap = texture(sampler2D(GLOBAL_TEXTURES[modelMaterials[gl_InstanceCustomIndexNV].normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate).xyz * 2.0f - 1.0f;

	//Sample the material properties.
	vec4 materialProperties = texture(sampler2D(GLOBAL_TEXTURES[modelMaterials[gl_InstanceCustomIndexNV].material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate);

	//Store the roughness, metallic, ambient occlusion and luminance.
	float roughness = materialProperties.x;
	float metallic = materialProperties.y;
	float ambientOcclusion = materialProperties.z;
	float luminance = materialProperties.w * modelMaterials[gl_InstanceCustomIndexNV].luminance_multiplier;

	//Calculate the tangent space matrix.
	mat3 tangentSpaceMatrix = mat3(finalVertex.tangent, cross(finalVertex.tangent, geometryNormal), geometryNormal);

	//Calculate the shading normal.
	vec3 shadingNormal = tangentSpaceMatrix * normalMap;

	//Determine if this ray hit a volumetric particle instead, and if so, alter the material properties so that that particle is lit instead.
	float volumetricHitDistance = mix(VIEW_DISTANCE, 0.0f, VOLUMETRIC_LIGHTING_DENSITY) * RandomFloat(vec2(gl_LaunchIDNV.xy), globalRandomSeed1);

	if (gl_HitTNV > volumetricHitDistance)
	{
		hitPosition = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * volumetricHitDistance;

		albedo = vec3(0.8f, 0.9f, 1.0f);
		geometryNormal = randomDirection;
		shadingNormal = randomDirection;
		roughness = 1.0f;
		metallic = 0.0f;
		ambientOcclusion = 1.0f;
		luminance = 0.0f;
	}

	//Calculate the radiance.
	vec3 radiance = vec3(0.0f);

	//Add the luminance lighting.
	radiance += albedo * luminance;

	//Calculate the indirect lighting.
	if (currentRecursionDepth < 1)
	{
		//Calculate the random irradiance direction.
		vec3 randomIrradianceDirection = normalize(mix(reflect(gl_WorldRayDirectionNV, shadingNormal), dot(geometryNormal, randomDirection) >= 0.0f ? randomDirection : randomDirection * -1.0f, CalculateDiffuseComponent(roughness, metallic)));

		//Set the current recursion depth.
		rayPayload.currentRecursionDepth = currentRecursionDepth + 1;

		//Fire the ray!
		traceNV(
				topLevelAccelerationStructure, 		//topLevel
				gl_RayFlagsOpaqueNV, 				//rayFlags
				0xff, 								//cullMask
				0, 									//sbtRecordOffset
				0, 									//sbtRecordStride
				0, 									//missIndex
				hitPosition, 						//origin
				CATALYST_RAY_TRACING_T_MINIMUM, 	//Tmin
				randomIrradianceDirection, 			//direction
				VIEW_DISTANCE, 						//Tmax
				0 									//payload
				);

		//Add the indirect lighting to the radiance.
		radiance += CalculateIndirectLighting(	gl_WorldRayDirectionNV,
												albedo,
												shadingNormal,
												roughness,
												metallic,
												ambientOcclusion,
												rayPayload.radiance);
	}

	//Calculate a random chosen light.
	Light light = UnpackLight(int(randomValue * float(numberOfLights)));

	switch (light.type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
		{
			//Determine the visibility.
			visibility = 0.0f;

			traceNV(
					topLevelAccelerationStructure, 																//topLevel
					gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
					0xff, 																						//cullMask
					0, 																							//sbtRecordOffset
					0, 																							//sbtRecordStride
					1, 																							//missIndex
					hitPosition, 																				//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
					-light.position_or_direction,																//direction
					CATALYST_RAY_TRACING_T_MAXIMUM,																//Tmax
					1 																							//payload
					);

			radiance += CalculateDirectLight(	-gl_WorldRayDirectionNV,
												-light.position_or_direction,
												albedo,
												shadingNormal,
												roughness,
												metallic,
												1.0f,
												1.0f,
												light.luminance * float(numberOfLights)) * visibility;

			break;
		}

		case LIGHT_TYPE_POINT:
		{
			vec3 randomLightPosition = light.position_or_direction + randomDirection * light.size;

			float lengthToLight = length(randomLightPosition - hitPosition);
			vec3 lightDirection = vec3(randomLightPosition - hitPosition) / lengthToLight;

			//Calculate the attenuation.
			float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

			//Determine the visibility.
			visibility = 0.0f;

			traceNV(
					topLevelAccelerationStructure, 																//topLevel
					gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
					0xff, 																						//cullMask
					0, 																							//sbtRecordOffset
					0, 																							//sbtRecordStride
					1, 																							//missIndex
					hitPosition, 																				//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
					lightDirection,																				//direction
					lengthToLight,																				//Tmax
					1 																							//payload
					);

			radiance += CalculateDirectLight(	-gl_WorldRayDirectionNV,
												lightDirection,
												albedo,
												shadingNormal,
												roughness,
												metallic,
												1.0f,
												1.0f,
												light.luminance * float(numberOfLights)) * attenuation * visibility;

			break;
		}
	}

	

	//Write to the ray payload.
	rayPayload.radiance = radiance;

	if (currentRecursionDepth == 0)
	{
		rayPayload.albedo = albedo;
		rayPayload.geometryNormal = geometryNormal;
		rayPayload.hitDistance = gl_HitTNV;
		rayPayload.shadingNormal = shadingNormal;
		rayPayload.materialProperties = modelMaterials[gl_InstanceCustomIndexNV].properties;
		rayPayload.roughness = roughness;
		rayPayload.metallic = metallic;
		rayPayload.ambientOcclusion = ambientOcclusion;
		rayPayload.luminance = luminance;
	}
	*/
}