//Includes.
//#include "CatalystTerrainUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_POSITION;
    layout (offset = 16) uint NORMAL_MAP_TEXTURE_INDEX;
    layout (offset = 20) uint INDEX_MAP_TEXTURE_INDEX;
    layout (offset = 24) uint BLEND_MAP_TEXTURE_INDEX;
    layout (offset = 28) float MAP_RESOLUTION;
    layout (offset = 32) float MAP_RESOLUTION_RECIPROCAL;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out vec3 fragment_world_position;
layout (location = 1) out vec2 fragment_height_map_texture_coordinate;

/*
*	Calculates the displacement.
*/
/*
float CalculateDisplacement(vec2 height_map_texture_coordinate, vec2 material_texture_coordinate)
{
	//Retrieve the 4 materials to blend between.
	vec4 index_map = texture(sampler2D(GLOBAL_TEXTURES[INDEX_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

	Material material_1 = GLOBAL_MATERIALS[uint(index_map[0] * 255.0f)];
	Material material_2 = GLOBAL_MATERIALS[uint(index_map[1] * 255.0f)];
	Material material_3 = GLOBAL_MATERIALS[uint(index_map[2] * 255.0f)];
	Material material_4 = GLOBAL_MATERIALS[uint(index_map[3] * 255.0f)];

	//Retrieve the 4 displacement values.
	float displacement_1 = 0.5f;
	float displacement_2 = 0.5f;
	float displacement_3 = 0.5f;
	float displacement_4 = 0.5f;

	if (TEST_BIT(material_1._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))
	{
		displacement_1 = max(texture(sampler2D(GLOBAL_TEXTURES[material_1._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w, TERRAIN_MINIMUM_DISPLACEMENT);
	}
	
	if (TEST_BIT(material_2._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))
	{
		displacement_2 = max(texture(sampler2D(GLOBAL_TEXTURES[material_2._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w, TERRAIN_MINIMUM_DISPLACEMENT);
	}

	if (TEST_BIT(material_3._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))
	{
		displacement_3 = max(texture(sampler2D(GLOBAL_TEXTURES[material_3._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w, TERRAIN_MINIMUM_DISPLACEMENT);
	}
	
	if (TEST_BIT(material_4._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))
	{
		displacement_4 = max(texture(sampler2D(GLOBAL_TEXTURES[material_4._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w, TERRAIN_MINIMUM_DISPLACEMENT);
	}

	//Retrieve the blend map.
	vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[BLEND_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

	//Alter the blend values based on the displacement values.
	blend_map[0] *= STRENGTHEN_DISPLACEMENT(displacement_1);
	blend_map[1] *= STRENGTHEN_DISPLACEMENT(displacement_2);
	blend_map[2] *= STRENGTHEN_DISPLACEMENT(displacement_3);
	blend_map[3] *= STRENGTHEN_DISPLACEMENT(displacement_4);

    //Renormalize the blend map.
    float inverse_sum = 1.0f / (blend_map[0] + blend_map[1] + blend_map[2] + blend_map[3]);

	blend_map[0] *= inverse_sum;
	blend_map[1] *= inverse_sum;
	blend_map[2] *= inverse_sum;
	blend_map[3] *= inverse_sum;

	//Blend the displacement.
	return 	displacement_1 * blend_map[0]
			+ displacement_2 * blend_map[1]
			+ displacement_3 * blend_map[2]
			+ displacement_4 * blend_map[3];
}
*/

void CatalystShaderMain()
{
	//Calculate the fragment world position.
	fragment_world_position = WORLD_POSITION + vertex_position;

	//Calculate the fragment height map texture coordinate.
	fragment_height_map_texture_coordinate = vertex_texture_coordinate;

	//Calculate the material texture coordinate.
	//vec2 material_texture_coordinate = fragment_world_position.xz * 0.25f;

	//Calculate the displacement.
	//float first_displacement 	= CalculateDisplacement(fragment_height_map_texture_coordinate, material_texture_coordinate);
	//float second_displacement	= CalculateDisplacement(fragment_height_map_texture_coordinate + vec2(0.0f, 1.0f) / MATERIAL_MAPS_RESOLOLUTION, material_texture_coordinate);
	//float third_displacement 	= CalculateDisplacement(fragment_height_map_texture_coordinate + vec2(1.0f, 0.0f) / MATERIAL_MAPS_RESOLOLUTION, material_texture_coordinate);
	//float fourth_displacement 	= CalculateDisplacement(fragment_height_map_texture_coordinate + vec2(1.0f, 1.0f) / MATERIAL_MAPS_RESOLOLUTION, material_texture_coordinate);

	//float blend_1 = mix(first_displacement, second_displacement, fract(fragment_height_map_texture_coordinate.y * MATERIAL_MAPS_RESOLOLUTION));
	//float blend_2 = mix(third_displacement, fourth_displacement, fract(fragment_height_map_texture_coordinate.y * MATERIAL_MAPS_RESOLOLUTION));

	//float final_displacement = mix(blend_1, blend_2, fract(fragment_height_map_texture_coordinate.x * MATERIAL_MAPS_RESOLOLUTION));

	//Apply the displacement.
	//fragment_world_position.y += mix(-0.5f, 0.5f, final_displacement);

	//Write the position.
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_world_position, 1.0f);
}