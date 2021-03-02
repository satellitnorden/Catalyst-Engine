//Includes.
#include "CatalystTerrainUtilities.glsl"

//Constants.
#define TERRAIN_MAP_RESOLUTION (33)

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 WORLD_TO_LIGHT_MATRIX;
	layout (offset = 64) vec3 WORLD_GRID_DELTA;
    layout (offset = 80) vec2 WORLD_POSITION;
    layout (offset = 88) float PATCH_SIZE;
    layout (offset = 92) int BORDERS;
    layout (offset = 96) float VERTEX_BORDER_OFFSET_FIRST;
    layout (offset = 100) float VERTEX_BORDER_OFFSET_SECOND;
    layout (offset = 104) uint HEIGHT_MAP_TEXTURE_INDEX;
    layout (offset = 108) uint INDEX_MAP_TEXTURE_INDEX;
};

//In parameters.
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in int vertex_borders;

/*
*	Calculates the displacement.
*/
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
	//vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_BLEND_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);
	vec4 blend_map = vec4(1.0f, 0.0f, 0.0f, 0.0f);

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

void CatalystShaderMain()
{
	//Set the position.
	vec2 position = vertex_position.xy;

	//Calculate the horizontal border offset multiplier.
	float isUpperMultiplier = float((vertex_borders & BIT(0)) & (BORDERS & BIT(0)));
	float isLowerMultiplier = float((vertex_borders & BIT(4)) & (BORDERS & BIT(4)));
	float horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	float isRightMultiplier = float((vertex_borders & BIT(2)) & (BORDERS & BIT(2)));
	float isLeftMultiplier = float((vertex_borders & BIT(6)) & (BORDERS & BIT(6)));
	float verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_FIRST * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_FIRST * verticalBorderOffsetWeight;

	//Calculate the horizontal border offset multiplier.
	isUpperMultiplier = float((vertex_borders & BIT(1)) & (BORDERS & BIT(1)));
	isLowerMultiplier = float((vertex_borders & BIT(5)) & (BORDERS & BIT(5)));
	horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	isRightMultiplier = float((vertex_borders & BIT(3)) & (BORDERS & BIT(3)));
	isLeftMultiplier = float((vertex_borders & BIT(7)) & (BORDERS & BIT(7)));
	verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_SECOND * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_SECOND * verticalBorderOffsetWeight;

	//Calculate the world position.
	vec3 world_position = vec3(WORLD_POSITION.x, 0.0f, WORLD_POSITION.y) + vec3(position.x, 0.0f, position.y) * PATCH_SIZE;

	//Calculate the height map texture coordinate.
	vec2 height_map_texture_coordinate = vertex_position.xy;

	//Apply the height.
	world_position.y += texture(sampler2D(GLOBAL_TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate).x;

	//Calculate the material texture coordinate.
	vec2 material_texture_coordinate = world_position.xz * 0.25f;

	//Calculate the displacement.
	float first_displacement 	= CalculateDisplacement(height_map_texture_coordinate, material_texture_coordinate);
	float second_displacement	= CalculateDisplacement(height_map_texture_coordinate + vec2(0.0f, 1.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);
	float third_displacement 	= CalculateDisplacement(height_map_texture_coordinate + vec2(1.0f, 0.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);
	float fourth_displacement 	= CalculateDisplacement(height_map_texture_coordinate + vec2(1.0f, 1.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);

	float blend_1 = mix(first_displacement, second_displacement, fract(height_map_texture_coordinate.y * TERRAIN_MAP_RESOLUTION));
	float blend_2 = mix(third_displacement, fourth_displacement, fract(height_map_texture_coordinate.y * TERRAIN_MAP_RESOLUTION));

	float final_displacement = mix(blend_1, blend_2, fract(height_map_texture_coordinate.x * TERRAIN_MAP_RESOLUTION));

	//Apply the displacement.
	world_position.y += mix(-0.5f, 0.5f, final_displacement);

	//Write the position.
	gl_Position = WORLD_TO_LIGHT_MATRIX * vec4(world_position + WORLD_GRID_DELTA, 1.0f);
}