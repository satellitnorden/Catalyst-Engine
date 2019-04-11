//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

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

  	vec4 vertexData1 = vertexBuffers[0].vertexData[VERTEX_SIZE * index + 0];
  	vec4 vertexData2 = vertexBuffers[0].vertexData[VERTEX_SIZE * index + 1];
  	vec4 vertexData3 = vertexBuffers[0].vertexData[VERTEX_SIZE * index + 2];

  	vertex.position = vertexData1.xyz;
  	vertex.normal = vec3(vertexData1.w, vertexData2.x, vertexData2.y);
  	vertex.tangent = vec3(vertexData2.z, vertexData2.w, vertexData3.x);
  	vertex.textureCoordinate = vec2(vertexData3.y, vertexData3.z);

  	return vertex;
}

void main()
{
	//Unpack the vertices making up the triangle.
	Vertex vertex1 = UnpackVertex(indexBuffers[0].indicesData[gl_PrimitiveID * 3]);
	Vertex vertex2 = UnpackVertex(indexBuffers[0].indicesData[gl_PrimitiveID * 3 + 1]);
	Vertex vertex3 = UnpackVertex(indexBuffers[0].indicesData[gl_PrimitiveID * 3 + 2]);

	//Calculate the final vertex using the barycentric coordinates.
	vec3 barycentricCoordinates = vec3(1.0f - hitAttribute.x - hitAttribute.y, hitAttribute.x, hitAttribute.y);

	Vertex finalVertex;

	finalVertex.position = vertex1.position * barycentricCoordinates.x + vertex2.position * barycentricCoordinates.y + vertex3.position * barycentricCoordinates.z;
	finalVertex.normal = vertex1.normal * barycentricCoordinates.x + vertex2.normal * barycentricCoordinates.y + vertex3.normal * barycentricCoordinates.z;
	finalVertex.tangent = vertex1.tangent * barycentricCoordinates.x + vertex2.tangent * barycentricCoordinates.y + vertex3.tangent * barycentricCoordinates.z;
	finalVertex.textureCoordinate = vertex1.textureCoordinate * barycentricCoordinates.x + vertex2.textureCoordinate * barycentricCoordinates.y + vertex3.textureCoordinate * barycentricCoordinates.z;

	hitValue = vec3(max(dot(finalVertex.normal, normalize(finalVertex.position + vec3(1.0f, 1.0f, 1.0f) - finalVertex.position)), 0.0f));
}
