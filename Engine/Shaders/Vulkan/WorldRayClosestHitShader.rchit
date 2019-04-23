//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Light struct definition.
struct Light
{
	vec3 color;
	vec3 position;
	float attenuationDistance;
	float size;
};

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
#define MAXIMUM_NUMBER_OF_MODELS (8)
#define MAXIMUM_NUMBER_OF_LIGHTS (4)
#define VERTEX_SIZE (3)

#define INDICES_OFFSET (2637)

//Descriptor set data.
layout (set = 1, binding = 1) uniform accelerationStructureNV topLevelAccelerationStructure;
layout (set = 1, binding = 2) uniform samplerCube environmentTexture;
layout (set = 1, binding = 3) buffer inputData1 { vec4 vertexData[]; } vertexBuffers[MAXIMUM_NUMBER_OF_MODELS];
layout (set = 1, binding = 4) buffer inputData2 { uint indicesData[]; } indexBuffers[MAXIMUM_NUMBER_OF_MODELS];
layout (std140, set = 1, binding = 5) uniform ModelUniformData
{
    layout (offset = 0) Material[MAXIMUM_NUMBER_OF_MODELS] modelMaterials;
};
layout (std140, set = 1, binding = 6) uniform LightUniformData
{
	layout (offset = 0) int numberOfLights;
    layout (offset = 16) vec4[MAXIMUM_NUMBER_OF_LIGHTS * 2] lightData;
};

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) int numberOfIterations;
	layout (offset = 4) int currentIteration;

    layout (offset = 8) float seed1;
    layout (offset = 12) float seed2;
    layout (offset = 16) float seed3;
    layout (offset = 20) float seed4;
    layout (offset = 24) float seed5;
    layout (offset = 28) float seed6;
    layout (offset = 32) float seed7;
    layout (offset = 36) float seed8;
    layout (offset = 40) float seed9;
};

//In parameters.
layout(location = 0) rayPayloadInNV RayPayload rayPayload;
hitAttributeNV vec3 hitAttribute;

/*
*	Unpacks the light at the given index.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 lightData1 = lightData[index * 2 + 0];
  	vec4 lightData2 = lightData[index * 2 + 1];

  	light.color = lightData1.xyz;
  	light.position = vec3(lightData1.w, lightData2.xy);
  	light.attenuationDistance = lightData2.z;
  	light.size = lightData2.w;

  	return light;
}

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
	//Indicate that there was a hit.
	rayPayload.hit = true;

	//Calculate the hit position.
	vec3 hitPosition = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV;

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

	//Store the current recursion depth.
	int currentRecursionDepth = rayPayload.currentRecursionDepth++;

	//Calculate the diffuse irradiance.
	vec3 diffuseIrradiance;

	vec3 randomDiffuseIrradianceDirection = normalize(vec3(	RandomFloat(vec3(gl_LaunchIDNV.xy, seed4)) * 2.0f - 1.0f,
															RandomFloat(vec3(gl_LaunchIDNV.xy, seed5)) * 2.0f - 1.0f,
															RandomFloat(vec3(gl_LaunchIDNV.xy, seed6)) * 2.0f - 1.0f));

	randomDiffuseIrradianceDirection *= dot(randomDiffuseIrradianceDirection, finalNormal) >= 0.0f ? 1.0f : -1.0f;

	vec3 diffuseIrradianceDirection = reflect(gl_WorldRayDirectionNV, randomDiffuseIrradianceDirection);

	if (currentRecursionDepth == 0)
	{
		traceNV(
				topLevelAccelerationStructure, 	//topLevel
				gl_RayFlagsOpaqueNV, 			//rayFlags
				0xff, 							//cullMask
				0, 								//sbtRecordOffset
				0, 								//sbtRecordStride
				0, 								//missIndex
				hitPosition, 					//origin
				CATALYST_RAY_TRACING_T_MINIMUM, //Tmin
				diffuseIrradianceDirection, 	//direction
				CATALYST_RAY_TRACING_T_MAXIMUM, //Tmax
				0 								//payload
				);

		diffuseIrradiance = rayPayload.radiance;
	}

	else
	{
		diffuseIrradiance = texture(environmentTexture, diffuseIrradianceDirection).rgb;
	}

	//Calculate the specular irradiance.
	vec3 specularIrradiance;

	vec3 specularIrradianceDirection = reflect(gl_WorldRayDirectionNV, finalNormal);

	if (currentRecursionDepth == 0)
	{
		traceNV(
				topLevelAccelerationStructure, 	//topLevel
				gl_RayFlagsOpaqueNV, 			//rayFlags
				0xff, 							//cullMask
				0, 								//sbtRecordOffset
				0, 								//sbtRecordStride
				0, 								//missIndex
				hitPosition, 					//origin
				CATALYST_RAY_TRACING_T_MINIMUM, //Tmin
				specularIrradianceDirection, 	//direction
				CATALYST_RAY_TRACING_T_MAXIMUM, //Tmax
				0 								//payload
				);

		specularIrradiance = rayPayload.radiance;
	}

	else
	{
		specularIrradiance = texture(environmentTexture, specularIrradianceDirection).rgb;
	}

	//Calculate the final radiance.
	vec3 finalRadiance = vec3(0.0f);

	//Start off with the ambient lighting.
	finalRadiance += CalculateAmbient(	albedo,
										diffuseIrradiance,
										finalNormal,
										specularIrradiance,
										normalize(gl_WorldRayOriginNV - hitPosition),
										materialProperties.z,
										materialProperties.y,
										materialProperties.x);

	//Calculate the directional light.
	vec3 randomLightDirection = normalize(vec3(	RandomFloat(vec3(gl_LaunchIDNV.xy, seed7)) * 2.0f - 1.0f,
												RandomFloat(vec3(gl_LaunchIDNV.xy, seed8)) * 2.0f - 1.0f,
												RandomFloat(vec3(gl_LaunchIDNV.xy, seed9)) * 2.0f - 1.0f));
	randomLightDirection *= dot(randomLightDirection, directionalLightDirection) >= 0.0f ? 1.0f : -1.0f;
	randomLightDirection = mix(directionalLightDirection, randomLightDirection, 0.025f);

	float shadowMultiplier = 1.0f;

	if (currentRecursionDepth == 0)
	{
		

		//Do the actual ray cast.
		traceNV(
				topLevelAccelerationStructure, 				//topLevel
				gl_RayFlagsOpaqueNV, 						//rayFlags
				0xff, 										//cullMask
				0, 											//sbtRecordOffset
				0, 											//sbtRecordStride
				0, 											//missIndex
				hitPosition, 								//origin
				CATALYST_RAY_TRACING_T_MINIMUM, 			//Tmin
				-randomLightDirection,						//direction
				CATALYST_RAY_TRACING_T_MAXIMUM,				//Tmax
				0 											//payload
				);

		shadowMultiplier = rayPayload.hit ? 0.0f : 1.0f;
	}

	finalRadiance += CalculateLight(normalize(gl_WorldRayOriginNV - hitPosition),
									-randomLightDirection,
									finalNormal,
									1.0f,
									materialProperties.x,
									materialProperties.y,
									albedo,
									directionalLightColor) * shadowMultiplier;

	//Write the final radiance.
	rayPayload.radiance = finalRadiance;
}
