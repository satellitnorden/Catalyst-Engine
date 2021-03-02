//Includes.
#include "CatalystModelCore.glsl"
#include "CatalystVegetationCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) uint MODEL_FLAGS;
    layout (offset = 20) float START_FADE_OUT_DISTANCE_SQUARED;
    layout (offset = 24) float END_FADE_OUT_DISTANCE_SQUARED;
    layout (offset = 28) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;
layout (location = 4) in mat4 vertex_transformation;

//Out parameters.
layout (location = 0) out mat3 fragment_tangent_space_matrix;
layout (location = 3) out vec3 fragment_world_position;
layout (location = 4) out vec2 fragment_texture_coordinate;
layout (location = 5) out float fragment_fade_out_opacity;

void CatalystShaderMain()
{
	//Calculate the tangent/bitangent/normal.
	vec3 tangent = normalize(vec3(vertex_transformation * vec4(vertex_tangent, 0.0f)));
	vec3 bitangent = normalize(vec3(vertex_transformation * vec4(cross(vertex_normal, vertex_tangent), 0.0f)));
	vec3 normal = normalize(vec3(vertex_transformation * vec4(vertex_normal, 0.0f)));

	//Pass data to the fragment shader.
	fragment_tangent_space_matrix = mat3(tangent, bitangent, normal);
	fragment_world_position = vec3(vertex_transformation * vec4(vertex_position, 1.0f)) + WORLD_GRID_DELTA;
	fragment_texture_coordinate = vertex_texture_coordinate;

	if (START_FADE_OUT_DISTANCE_SQUARED < FLOAT32_MAXIMUM)
	{
		fragment_fade_out_opacity = 1.0f - (clamp(LengthSquared3(fragment_world_position - PERCEIVER_WORLD_POSITION) - START_FADE_OUT_DISTANCE_SQUARED, 0.0f, END_FADE_OUT_DISTANCE_SQUARED - START_FADE_OUT_DISTANCE_SQUARED) / (END_FADE_OUT_DISTANCE_SQUARED - START_FADE_OUT_DISTANCE_SQUARED));
	}

	else
	{
		fragment_fade_out_opacity = 1.0f;
	}

	//Apply model flags, if necessary.
	if (TEST_BIT(MODEL_FLAGS, MODEL_FLAG_IS_VEGETATION))
	{
		//Apply the wind displacement.
		fragment_world_position += CalculateWindDisplacement(vertex_transformation[3].xyz, fragment_world_position, normal, 0.5f, 0.5f, 0.5f) * vertex_position.y;
	}

	if (TEST_BIT(MODEL_FLAGS, MODEL_FLAG_SNAP_TO_TERRAIN))
	{
		/*
		//Calculate the absolute world position.
		vec3 absolute_world_position = fragment_world_position + (vec3(CURRENT_WORLD_GRID_CELL) * WORLD_GRID_SIZE);

		//Calculate the height map texture coordinate.
		vec2 height_map_texture_coordinate = (absolute_world_position.xz - HEIGHT_MAP_COORDINATE_OFFSET + (TERRAIN_MAP_RESOLUTION * 0.5f)) / TERRAIN_MAP_RESOLUTION;

		//Retrieve the terrain height.
		float terrain_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate).x;

		//Apply the difference.
		fragment_world_position.y = terrain_height + vertex_position.y + WORLD_GRID_DELTA.y;
		*/
	}

	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_world_position, 1.0f);
}