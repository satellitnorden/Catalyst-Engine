//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystModelData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Constants.
#define VOLUMETRIC_SAMPLES (1)
#define VOLUMETRIC_DENSITY (0.0025f)

//Descriptor set data.
layout (set = 1, binding = 9) uniform accelerationStructureNV topLevelAccelerationStructure;
layout (set = 1, binding = 10) uniform samplerCube environmentTexture;

//In parameters.
layout(location = 0) rayPayloadInNV PrimaryRayPayload rayPayload;
layout(location = 1) rayPayloadInNV float visibility;
hitAttributeNV vec3 hitAttribute;

void main()
{
	//Store the current recursion depth.
	int currentRecursionDepth = rayPayload.currentRecursionDepth;

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

	//Calculate the hit position.
	vec3 hitPosition = finalVertex.position + finalVertex.normal * 0.00001f;

	//Calculate the mip level.
	float mipLevel = 0.0f; //TODO. ):

	//Sample the albedo.
	vec3 albedo = pow(textureLod(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].firstTextureIndex], finalVertex.textureCoordinate, mipLevel).rgb, vec3(2.2f));

	//Sample the normal map.
	vec3 normalMap = textureLod(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].secondTextureIndex], finalVertex.textureCoordinate, mipLevel).xyz * 2.0f - 1.0f;

	//Sample the material properties.
	vec4 materialProperties = textureLod(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].thirdTextureIndex], finalVertex.textureCoordinate, mipLevel);

	//Store the roughness, metallic, ambient occlusion and luminance.
	float roughness = materialProperties.x;
	float metallic = materialProperties.y;
	float ambientOcclusion = materialProperties.z;
	float luminance = materialProperties.w;

	//Calculate the tangent space matrix.
	mat3 tangentSpaceMatrix = mat3(finalVertex.tangent, cross(finalVertex.tangent, finalVertex.normal), finalVertex.normal);

	//Calculate the final normal.
	vec3 finalNormal = tangentSpaceMatrix * normalMap;

	//If this is a "translucent" material, modify some properties to make it appear that way.
	if ((modelMaterials[gl_InstanceCustomIndexNV].properties & MATERIAL_TRANSLUCENT_BIT) == MATERIAL_TRANSLUCENT_BIT)
	{
		albedo = texture(environmentTexture, refract(gl_WorldRayDirectionNV, finalNormal, 0.5f)).rgb;
		roughness = 0.0f;
		metallic = 1.0f;
		ambientOcclusion = 1.0f;
		luminance = 1.0f;
	}

	//Calculate the direct lighting.
	vec3 directLighting = vec3(0.0f);

	//Add the luminance lighting.
	directLighting += albedo * luminance * modelMaterials[gl_InstanceCustomIndexNV].luminanceMultiplier;

	//Add the highlight.
	directLighting += CalculateHighlight(gl_WorldRayDirectionNV, finalNormal, modelMaterials[gl_InstanceCustomIndexNV].properties);

	//Calculate the random direction.
	vec3 randomDirection = normalize(rayPayload.randomVector.xyz * 2.0f - 1.0f);

	//Calculate the irradiance.
	vec3 diffuseIrradiance = vec3(0.0f);
	vec3 specularIrradiance = vec3(0.0f);

	if (currentRecursionDepth == 0)
	{
		{
			//Add the diffuse irradiance.
			vec3 randomDiffuseIrradianceDirection = dot(randomDirection, finalVertex.normal) >= 0.0f ? randomDirection : randomDirection * -1.0f;

			rayPayload.currentRecursionDepth = currentRecursionDepth + 1;

			traceNV(
					topLevelAccelerationStructure, 		//topLevel
					gl_RayFlagsOpaqueNV, 				//rayFlags
					0xff, 								//cullMask
					0, 									//sbtRecordOffset
					0, 									//sbtRecordStride
					0, 									//missIndex
					hitPosition, 						//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 	//Tmin
					randomDiffuseIrradianceDirection, 	//direction
					CATALYST_RAY_TRACING_T_MAXIMUM, 	//Tmax
					0 									//payload
					);

			diffuseIrradiance = rayPayload.directLighting;
		}

		{
			//Add the specular irradiance.
			vec3 specularIrradianceDirection = reflect(gl_WorldRayDirectionNV, finalNormal);

			rayPayload.currentRecursionDepth = currentRecursionDepth + 1;

			traceNV(
					topLevelAccelerationStructure, 		//topLevel
					gl_RayFlagsOpaqueNV, 				//rayFlags
					0xff, 								//cullMask
					0, 									//sbtRecordOffset
					0, 									//sbtRecordStride
					0, 									//missIndex
					hitPosition, 						//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 	//Tmin
					specularIrradianceDirection, 	//direction
					CATALYST_RAY_TRACING_T_MAXIMUM, 	//Tmax
					0 									//payload
					);

			specularIrradiance = rayPayload.directLighting;
		}
	}

	//Calculate volumetric lighting.
	vec3 volumetricLighting = vec3(0.0f);

	//Calculate a randomly chosen light.
	for (int i = 0; i < numberOfLights; ++i)
	{
		Light light = UnpackLight(i);

		vec3 randomLightPosition = light.position/* + randomDirection * rayPayload.randomVector.w * light.size*/;

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

		directLighting += CalculateDirectLight(	-gl_WorldRayDirectionNV,
												lightDirection,
												albedo,
												finalNormal,
												roughness,
												metallic,
												light.color * light.strength) * attenuation * visibility;

		//Also calculate some volumetric lighting for this light.
		if (currentRecursionDepth == 0)
		{
			vec3 randomPositionAlongRay = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV * fract(rayPayload.randomVector.w);

			lengthToLight = length(randomLightPosition - randomPositionAlongRay);
			lightDirection = vec3(randomLightPosition - randomPositionAlongRay) / lengthToLight;

			visibility = 0.0f;

			traceNV(
					topLevelAccelerationStructure, 																//topLevel
					gl_RayFlagsTerminateOnFirstHitNV | gl_RayFlagsOpaqueNV | gl_RayFlagsSkipClosestHitShaderNV, //rayFlags
					0xff, 																						//cullMask
					0, 																							//sbtRecordOffset
					0, 																							//sbtRecordStride
					1, 																							//missIndex
					randomPositionAlongRay, 																	//origin
					CATALYST_RAY_TRACING_T_MINIMUM, 															//Tmin
					lightDirection,																				//direction
					lengthToLight,																				//Tmax
					1 																							//payload
					);

			volumetricLighting += light.color * light.strength * VOLUMETRIC_DENSITY * visibility;
		}
	}

	//Write to the ray payload.
	rayPayload.diffuseIrradiance = diffuseIrradiance;
	rayPayload.specularIrradiance = specularIrradiance;
	rayPayload.directLighting = directLighting;
	rayPayload.volumetricLighting = volumetricLighting;
		
	if (currentRecursionDepth == 0)
	{
		rayPayload.albedo = albedo;
		rayPayload.geometryNormal = finalVertex.normal;
		rayPayload.shadingNormal = finalNormal;
		rayPayload.depth = gl_HitTNV;
		rayPayload.materialProperties = modelMaterials[gl_InstanceCustomIndexNV].properties;
		rayPayload.roughness = roughness;
		rayPayload.metallic = metallic;
		rayPayload.ambientOcclusion = ambientOcclusion;
		rayPayload.luminance = luminance;
	}
}
