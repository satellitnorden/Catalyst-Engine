#if !defined(CATALYST_MODEL_DATA)
#define CATALYST_MODEL_DATA

//Constants.
#define MAXIMUM_NUMBER_OF_MODELS (512)
#define VERTEX_SIZE (3)

//Material struct definition.
struct Material
{
  int properties;
  int firstTextureIndex;
  int secondTextureIndex;
  int thirdTextureIndex;
  float luminanceMultiplier;
  int padding1;
  int padding2;
  int padding3;
};

//Vertex struct definition.
struct Vertex
{
  vec3 position;
  vec3 normal;
  vec3 tangent;
  vec2 textureCoordinate;
};

//Model uniform data.
layout (set = 3, binding = 0) buffer inputData1 { vec4 vertexData[]; } vertexBuffers[MAXIMUM_NUMBER_OF_MODELS];
layout (set = 3, binding = 1) buffer inputData2 { uint indicesData[]; } indexBuffers[MAXIMUM_NUMBER_OF_MODELS];
layout (std140, set = 3, binding = 2) uniform ModelUniformData
{
    layout (offset = 0) Material[MAXIMUM_NUMBER_OF_MODELS] modelMaterials;
};

/*
* Unpacks the vertex at the given index.
*/
Vertex UnpackVertex(int instanceIndex, uint vertexIndex)
{
  Vertex vertex;

  vec4 vertexData1 = vertexBuffers[instanceIndex].vertexData[VERTEX_SIZE * vertexIndex + 0];
  vec4 vertexData2 = vertexBuffers[instanceIndex].vertexData[VERTEX_SIZE * vertexIndex + 1];
  vec4 vertexData3 = vertexBuffers[instanceIndex].vertexData[VERTEX_SIZE * vertexIndex + 2];

  vertex.position = vertexData1.xyz;
  vertex.normal = vec3(vertexData1.w, vertexData2.x, vertexData2.y);
  vertex.tangent = vec3(vertexData2.z, vertexData2.w, vertexData3.x);
  vertex.textureCoordinate = vec2(vertexData3.y, vertexData3.z);

  return vertex;
}

#endif