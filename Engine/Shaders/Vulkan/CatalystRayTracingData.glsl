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

//Terrain render data table data.
layout (set = 1, binding = 0) uniform accelerationStructureNV TERRAIN_TOP_LEVEL_ACCELERATION_STRUCTURE;
//Static models render data table data.
layout (set = 1, binding = 1) uniform accelerationStructureNV STATIC_TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 1, binding = 2) buffer STATIC_MODEL_VERTEX_DATA_BUFFER { vec4 STATIC_MODEL_VERTEX_DATA[]; } STATIC_MODEL_VERTEX_BUFFERS[];
layout (set = 1, binding = 3) buffer STATIC_MODEL_INDEX_DATA_BUFFER { uint STATIC_MODEL_INDEX_DATA[]; } STATIC_MODEL_INDEX_BUFFERS[];
layout (std140, set = 1, binding = 4) uniform STATIC_MODEL_UNIFORM_DATA
{
    layout (offset = 0) uvec4[MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS / 4] STATIC_MODEL_MATERIAL_INDICES;
};
//Dynamic models render data table data.
layout (set = 1, binding = 5) uniform accelerationStructureNV DYNAMIC_TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 1, binding = 6) buffer DYNAMIC_MODEL_VERTEX_DATA_BUFFER { vec4 DYNAMIC_MODEL_VERTEX_DATA[]; } DYNAMIC_MODEL_VERTEX_BUFFERS[];
layout (set = 1, binding = 7) buffer DYNAMIC_MODEL_INDEX_DATA_BUFFER { uint DYNAMIC_MODEL_INDEX_DATA[]; } DYNAMIC_MODEL_INDEX_BUFFERS[];
layout (std140, set = 1, binding = 8) uniform DYNAMIC_MODEL_UNIFORM_DATA
{
    layout (offset = 0) uvec4[MAXIMUM_NUMBER_OF_RAY_TRACED_DYNAMIC_MODELS / 4] DYNAMIC_MODEL_MATERIAL_INDICES;
};

/*
*	Unpacks the static model vertex at the given index.
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

/*
*	Unpacks the static model material index at the given instance index.
*/
uint UnpackStaticModelMaterialindex(uint instance_index)
{
	return STATIC_MODEL_MATERIAL_INDICES[instance_index / 4][instance_index & 3];
}

/*
*	Unpacks the dynamic model vertex at the given index.
*/
Vertex UnpackDynamicModelVertex(uint instance_index, uint vertex_index)
{
	Vertex vertex;

	vec4 vertex_data_1 = DYNAMIC_MODEL_VERTEX_BUFFERS[instance_index].DYNAMIC_MODEL_VERTEX_DATA[VERTEX_SIZE * vertex_index + 0];
	vec4 vertex_data_2 = DYNAMIC_MODEL_VERTEX_BUFFERS[instance_index].DYNAMIC_MODEL_VERTEX_DATA[VERTEX_SIZE * vertex_index + 1];
	vec4 vertex_data_3 = DYNAMIC_MODEL_VERTEX_BUFFERS[instance_index].DYNAMIC_MODEL_VERTEX_DATA[VERTEX_SIZE * vertex_index + 2];

	vertex.position = vertex_data_1.xyz;
	vertex.normal = vec3(vertex_data_1.w, vertex_data_2.x, vertex_data_2.y);
	vertex.tangent = vec3(vertex_data_2.z, vertex_data_2.w, vertex_data_3.x);
	vertex.texture_coordinate = vec2(vertex_data_3.y, vertex_data_3.z);

	return vertex;
}

/*
*	Unpacks the dynamic model material index at the given instance index.
*/
uint UnpackDynamicModelMaterialindex(uint instance_index)
{
	return DYNAMIC_MODEL_MATERIAL_INDICES[instance_index / 4][instance_index & 3];
}
#endif