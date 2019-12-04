//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#define RAY_TRACING_SHADER
#include "CatalystModelData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Constants.
#define VOLUMETRIC_LIGHTING_DENSITY (0.0f)

//In parameters.
layout(location = 0) rayPayloadInNV PathTracingRayPayload rayPayload;
layout(location = 1) rayPayloadInNV float visibility;
hitAttributeNV vec3 hitAttribute;

void main()
{
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
	vec3 albedo = pow(texture(sampler2D(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].albedo_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate).rgb, vec3(2.2f));

	//Sample the normal map.
	vec3 normalMap = texture(sampler2D(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].normal_map_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate).xyz * 2.0f - 1.0f;

	//Sample the material properties.
	vec4 materialProperties = texture(sampler2D(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].material_properties_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate);

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
}