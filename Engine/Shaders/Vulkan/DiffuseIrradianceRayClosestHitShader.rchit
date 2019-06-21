//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystModelData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//In parameters.
layout(location = 0) rayPayloadInNV DiffuseIrradianceRayPayload rayPayload;
layout(location = 1) rayPayloadInNV float visibility;
hitAttributeNV vec3 hitAttribute;

void main()
{
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

	//Sample the albedo.
	vec3 albedo = pow(texture(sampler2D(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].firstTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate).rgb, vec3(2.2f));

	//Sample the normal map.
	vec3 normalMap = texture(sampler2D(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].secondTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate).xyz * 2.0f - 1.0f;

	//Sample the material properties.
	vec4 materialProperties = texture(sampler2D(globalTextures[modelMaterials[gl_InstanceCustomIndexNV].thirdTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), finalVertex.textureCoordinate);

	//Store the roughness, metallic, ambient occlusion and luminance.
	float roughness = materialProperties.x;
	float metallic = materialProperties.y;
	float ambientOcclusion = materialProperties.z;
	float luminance = materialProperties.w * modelMaterials[gl_InstanceCustomIndexNV].luminanceMultiplier;

	//Calculate the tangent space matrix.
	mat3 tangentSpaceMatrix = mat3(finalVertex.tangent, cross(finalVertex.tangent, finalVertex.normal), finalVertex.normal);

	//Calculate the final normal.
	vec3 finalNormal = tangentSpaceMatrix * normalMap;

	//Calculate the highlight weight of this material and modify the material properties based on that.
	float highlightWeight = max(CalculateHighlightWeight(gl_WorldRayDirectionNV, finalNormal, modelMaterials[gl_InstanceCustomIndexNV].properties), 0.0f);

	albedo = mix(albedo, HIGHLIGHT_COLOR, highlightWeight);
	luminance = mix(luminance, luminance + 1.0f, highlightWeight);

	//Calculate the direct lighting.
	vec3 directLighting = vec3(0.0f);

	//Add the luminance lighting.
	directLighting += albedo * luminance;

	//Calculate a randomly chosen light.
	Light light = UnpackLight(int(rayPayload.randomVector.w * float(numberOfLights)));

	float lengthToLight = length(light.position - hitPosition);
	vec3 lightDirection = vec3(light.position - hitPosition) / lengthToLight;

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
											light.color * light.strength * float(numberOfLights)) * attenuation * visibility;

	//Write to the ray payload.
	rayPayload.directLighting = directLighting;
}