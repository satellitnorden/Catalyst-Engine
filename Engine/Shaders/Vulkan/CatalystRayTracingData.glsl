#if !defined(CATALYST_RAY_TRACING_DATA)
#define CATALYST_RAY_TRACING_DATA

//Constants.
#define VERTEX_SIZE (3)

//Vertex struct definition.
struct Vertex
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 texture_coordinate;
};

//Render data table data.
layout (set = 1, binding = 0) uniform accelerationStructureNV TERRAIN_TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 1, binding = 1) uniform accelerationStructureNV STATIC_TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 1, binding = 2) buffer STATIC_MODEL_VERTEX_DATA_BUFFER { vec4 STATIC_MODEL_VERTEX_DATA[]; } STATIC_MODEL_VERTEX_BUFFERS[MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS];
layout (set = 1, binding = 3) buffer STATIC_MODEL_INDEX_DATA_BUFFER { uint STATIC_MODEL_INDEX_DATA[]; } STATIC_MODEL_INDEX_BUFFERS[MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS];

/*
* Unpacks the static model vertex at the given index.
*/
Vertex UnpackStaticModelVertex(uint instance_index, uint vertex_index)
{
	Vertex vertex;

	vec4 vertex_data_1 = STATIC_MODEL_VERTEX_BUFFERS[instance_index].STATIC_MODEL_VERTEX_DATA[VERTEX_SIZE * vertex_index + 0];
	vec4 vertex_data_2 = STATIC_MODEL_VERTEX_BUFFERS[instance_index].STATIC_MODEL_VERTEX_DATA[VERTEX_SIZE * vertex_index + 1];
	vec4 vertex_data_3 = STATIC_MODEL_VERTEX_BUFFERS[instance_index].STATIC_MODEL_VERTEX_DATA[VERTEX_SIZE * vertex_index + 2];

	vertex.position = vertex_data_1.xyz;
	vertex.normal = vec3(vertex_data_1.w, vertex_data_2.x, vertex_data_2.y);
	vertex.tangent = vec3(vertex_data_2.z, vertex_data_2.w, vertex_data_3.x);
	vertex.texture_coordinate = vec2(vertex_data_3.y, vertex_data_3.z);

	return vertex;
}
#endif