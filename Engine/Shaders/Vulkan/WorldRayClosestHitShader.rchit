//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Vertex struct definition.
struct Vertex
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 textureCoordinate;
};

//Constants.
#define MAXIMUM_NUMBER_OF_VERTEX_BUFFERS (4)
#define VERTEX_SIZE (3)

#define INDICES_OFFSET (2637)

//Descriptor set data.
layout (set = 1, binding = 3) buffer inputData1 { vec4 vertexData[]; } vertexBuffers[MAXIMUM_NUMBER_OF_VERTEX_BUFFERS];
layout (set = 1, binding = 4) buffer inputData2 { uint indicesData[]; } indexBuffers[MAXIMUM_NUMBER_OF_VERTEX_BUFFERS];

//In parameters.
layout(location = 0) rayPayloadInNV vec3 hitValue;
hitAttributeNV vec3 hitAttribute;

/*
*	Unpacks the vertex at the given position.
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
	vec3 albedo = texture(globalTextures[3], finalVertex.textureCoordinate).rgb;

	//Sample the normal map.
	vec3 normalMap = texture(globalTextures[4], finalVertex.textureCoordinate).xyz * 2.0f - 1.0f;

	//Sample the material properties.
	vec4 materialProperties = texture(globalTextures[5], finalVertex.textureCoordinate);

	//Calculate the tangent space matrix.
	mat3 tangentSpaceMatrix = mat3(finalVertex.tangent, cross(finalVertex.tangent, finalVertex.normal), finalVertex.normal);

	//Calculate the final normal.
	vec3 finalNormal = tangentSpaceMatrix * normalMap;

	hitValue = CalculateLight(	normalize(perceiverWorldPosition - hitPosition),
								normalize(vec3(2.5f, 2.5f, 2.5f) - hitPosition),
								finalNormal,
								1.0f,
								materialProperties.x,
								materialProperties.y,
								albedo,
								vec3(1.0f, 0.9f, 0.8f) * 2.5f);
}
