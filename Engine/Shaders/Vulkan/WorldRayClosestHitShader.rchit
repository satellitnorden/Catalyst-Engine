//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystLightingData.glsl"

//Material struct definition.
struct Material
{
	int type;
	int firstTextureIndex;
	int secondTextureIndex;
	int thirdTextureIndex;	
};

//Vertex struct definition.
struct Vertex
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 textureCoordinate;
};

//Constants.
#define MAXIMUM_NUMBER_OF_MODELS (64)
#define VERTEX_SIZE (3)

//Descriptor set data.
layout (set = 1, binding = 5) uniform accelerationStructureNV topLevelAccelerationStructure;
layout (set = 1, binding = 7) buffer inputData1 { vec4 vertexData[]; } vertexBuffers[MAXIMUM_NUMBER_OF_MODELS];
layout (set = 1, binding = 8) buffer inputData2 { uint indicesData[]; } indexBuffers[MAXIMUM_NUMBER_OF_MODELS];
layout (std140, set = 1, binding = 9) uniform ModelUniformData
{
    layout (offset = 0) Material[MAXIMUM_NUMBER_OF_MODELS] modelMaterials;
};

//In parameters.
layout(location = 0) rayPayloadInNV PrimaryRayPayload rayPayload;
layout(location = 1) rayPayloadInNV float visibility;
hitAttributeNV vec3 hitAttribute;

/*
*	Unpacks the vertex at the given index.
*/
Vertex UnpackVertex(uint index)
{
	Vertex vertex;

  	vec4 vertexData1 = vertexBuffers[gl_InstanceCustomIndexNV].vertexData[VERTEX_SIZE * index + 0];
  	vec4 vertexData2 = vertexBuffers[gl_InstanceCustomIndexNV].vertexData[VERTEX_SIZE * index + 1];
  	vec4 vertexData3 = vertexBuffers[gl_InstanceCustomIndexNV].vertexData[VERTEX_SIZE * index + 2];

  	vertex.position = vertexData1.xyz;
  	vertex.normal = vec3(vertexData1.w, vertexData2.x, vertexData2.y);
  	vertex.tangent = vec3(vertexData2.z, vertexData2.w, vertexData3.x);
  	vertex.textureCoordinate = vec2(vertexData3.y, vertexData3.z);

  	return vertex;
}

void main()
{
	//Store the current recursion depth.
	int currentRecursionDepth = rayPayload.currentRecursionDepth++;

	//Unpack the vertices making up the triangle.
	Vertex vertex1 = UnpackVertex(indexBuffers[gl_InstanceCustomIndexNV].indicesData[gl_PrimitiveID * 3]);
	Vertex vertex2 = UnpackVertex(indexBuffers[gl_InstanceCustomIndexNV].indicesData[gl_PrimitiveID * 3 + 1]);
	Vertex vertex3 = UnpackVertex(indexBuffers[gl_InstanceCustomIndexNV].indicesData[gl_PrimitiveID * 3 + 2]);

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

	//Sample the albedo.
	vec3 albedo = texture(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].firstTextureIndex], finalVertex.textureCoordinate).rgb;

	//Sample the normal map.
	vec3 normalMap = texture(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].secondTextureIndex], finalVertex.textureCoordinate).xyz * 2.0f - 1.0f;

	//Sample the material properties.
	vec4 materialProperties = texture(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].thirdTextureIndex], finalVertex.textureCoordinate);

	//Store the roughness, metallic and ambient occlusion.
	float roughness = materialProperties.x;
	float metallic = materialProperties.y;
	float ambientOcclusion = materialProperties.z;

	//Calculate the tangent space matrix.
	mat3 tangentSpaceMatrix = mat3(finalVertex.tangent, cross(finalVertex.tangent, finalVertex.normal), finalVertex.normal);

	//Calculate the final normal.
	vec3 finalNormal = tangentSpaceMatrix * normalMap;

	//Calculate the indirect lighting.
	vec3 indirectLighting = vec3(0.0f);

	//Don't go below the maximum recursion depth.
	if (currentRecursionDepth < (CATALYST_RAY_TRACING_MAXIMUM_DEPTH - 1))
	{
		vec3 randomIrradianceDirection = rayPayload.randomVector;
		float randomIrradianceDirectionDot = dot(randomIrradianceDirection, finalVertex.normal);
		randomIrradianceDirection = randomIrradianceDirectionDot >= 0.0f ? randomIrradianceDirection : randomIrradianceDirection * -1.0f;
		randomIrradianceDirection = normalize(mix(finalNormal, randomIrradianceDirection, GetSpecularComponent(roughness, metallic)));
		randomIrradianceDirection = normalize(reflect(gl_WorldRayDirectionNV, randomIrradianceDirection));

		traceNV(
				topLevelAccelerationStructure, 	//topLevel
				gl_RayFlagsOpaqueNV, 			//rayFlags
				0xff, 							//cullMask
				0, 								//sbtRecordOffset
				0, 								//sbtRecordStride
				0, 								//missIndex
				hitPosition, 					//origin
				CATALYST_RAY_TRACING_T_MINIMUM, //Tmin
				randomIrradianceDirection, 		//direction
				CATALYST_RAY_TRACING_T_MAXIMUM, //Tmax
				0 								//payload
				);

		indirectLighting = CalculateIndirectLight(	-gl_WorldRayDirectionNV,
													randomIrradianceDirection,
													albedo,
													finalNormal,
													roughness,
													metallic,
													ambientOcclusion,
													rayPayload.indirectLighting + rayPayload.directLighting);
	}
	
	//Calculate the direct lighting.
	vec3 directLighting = vec3(0.0f);

	//Calculate the directional light.
	{
		vec3 randomLightDirection = rayPayload.randomVector;
		randomLightDirection *= dot(randomLightDirection, directionalLightDirection) >= 0.0f ? 1.0f : -1.0f;
		randomLightDirection = mix(directionalLightDirection, randomLightDirection, 0.0175f); //0.0025f step.

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
				-randomLightDirection,																		//direction
				CATALYST_RAY_TRACING_T_MAXIMUM,																//Tmax
				1 																							//payload
				);

		//Calculate the directional light lighting.
		vec3 directionalLightLighting = CalculateDirectLight(	-gl_WorldRayDirectionNV,
																-randomLightDirection,
																albedo,
																finalNormal,
																roughness,
																metallic,
																directionalLightColor * directionalLightIntensity);

		//Add it the the direct lighting term.
		directLighting += directionalLightLighting * visibility;

		//Write the directional light visibility to the texture.
		imageStore(directionalLightVisibilityTexture, ivec2(gl_LaunchIDNV.xy), vec4(visibility));
	}

	//Calculate all other lights.
	for (int i = 0; i < numberOfLights; ++i)
	{
		Light light = UnpackLight(i);

		vec3 randomLightPosition = light.position + rayPayload.randomVector * light.size;

		float lengthToLight = length(randomLightPosition - hitPosition);
		vec3 lightDirection = vec3(randomLightPosition - hitPosition) / lengthToLight;

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

		//Calculate the attenuation distance.
		float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

		//Calculate the lighting.
		vec3 lighting = CalculateDirectLight(	-gl_WorldRayDirectionNV,
												lightDirection,
												albedo,
												finalNormal,
												roughness,
												metallic,
												light.color * light.strength) * attenuation;

		directLighting += lighting * visibility;

		//Write the light direct lighting result to the texture.
		imageStore(lightsVisibilityTextures[i], ivec2(gl_LaunchIDNV.xy), vec4(visibility));
	}
	
	//Write to the ray payload.
	rayPayload.indirectLighting = indirectLighting;
	rayPayload.directLighting = directLighting;
	rayPayload.albedo = albedo;
	rayPayload.normal = finalNormal;
	rayPayload.depth = gl_HitTNV;
	rayPayload.roughness = roughness;
	rayPayload.metallic = metallic;
	rayPayload.ambientOcclusion = ambientOcclusion;
	rayPayload.hitInstanceID = gl_InstanceCustomIndexNV;
	rayPayload.hitPrimitiveID = gl_PrimitiveID;
}
