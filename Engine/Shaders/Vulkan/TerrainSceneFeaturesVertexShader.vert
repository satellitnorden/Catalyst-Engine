//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define VERTEX_BORDER_OFFSET_FIRST (1.0f / (64.0f))
#define VERTEX_BORDER_OFFSET_SECOND (1.0f / (32.0f))
#define STRENGTHEN_DISPLACEMENT(X) (X * X * X)

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 world_position;
    layout (offset = 8) float patch_size;
    layout (offset = 12) int borders;
};

//In parameters.
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in int vertexBorders;

//Out parameters.
layout (location = 0) out vec3 fragmentWorldPosition;
layout (location = 1) out vec2 fragment_height_map_texture_coordinate;

/*
*	Calculates the displacement.
*/
float CalculateDisplacement(vec2 height_map_texture_coordinate, vec2 material_texture_coordinate)
{
	//Retrieve the 4 materials to blend between.
	vec4 index_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_INDEX_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

	Material material_1 = GLOBAL_MATERIALS[int(index_map[0] * 255.0f)];
	Material material_2 = GLOBAL_MATERIALS[int(index_map[1] * 255.0f)];
	Material material_3 = GLOBAL_MATERIALS[int(index_map[2] * 255.0f)];
	Material material_4 = GLOBAL_MATERIALS[int(index_map[3] * 255.0f)];

	//Retrieve the 4 displacement values.
	float displacement_1 = texture(sampler2D(GLOBAL_TEXTURES[material_1.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_2 = texture(sampler2D(GLOBAL_TEXTURES[material_2.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_3 = texture(sampler2D(GLOBAL_TEXTURES[material_3.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_4 = texture(sampler2D(GLOBAL_TEXTURES[material_4.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;

	//Retrieve the blend map.
	vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_BLEND_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

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

void main()
{
	//Set the position.
	vec2 position = vertexPosition.xy;

	//Calculate the horizontal border offset multiplier.
	float isUpperMultiplier = float((vertexBorders & BIT(0)) & (borders & BIT(0)));
	float isLowerMultiplier = float((vertexBorders & BIT(4)) & (borders & BIT(4)));
	float horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	float isRightMultiplier = float((vertexBorders & BIT(2)) & (borders & BIT(2)));
	float isLeftMultiplier = float((vertexBorders & BIT(6)) & (borders & BIT(6)));
	float verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_FIRST * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_FIRST * verticalBorderOffsetWeight;

	//Calculate the horizontal border offset multiplier.
	isUpperMultiplier = float((vertexBorders & BIT(1)) & (borders & BIT(1)));
	isLowerMultiplier = float((vertexBorders & BIT(5)) & (borders & BIT(5)));
	horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	isRightMultiplier = float((vertexBorders & BIT(3)) & (borders & BIT(3)));
	isLeftMultiplier = float((vertexBorders & BIT(7)) & (borders & BIT(7)));
	verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_SECOND * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_SECOND * verticalBorderOffsetWeight;

	//Write parameters to the fragment shader.
	fragmentWorldPosition = vec3(world_position.x, 0.0f, world_position.y) + vec3(position.x, 0.0f, position.y) * patch_size;
	fragment_height_map_texture_coordinate = (fragmentWorldPosition.xz + (TERRAIN_MAP_RESOLUTION * 0.5f)) / TERRAIN_MAP_RESOLUTION;

	//Apply the height.
	fragmentWorldPosition.y += texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate).x;

	//Calculate the material texture coordinate.
	vec2 material_texture_coordinate = fragmentWorldPosition.xz * 0.25f;

	//Apply the displacement.
	float displacement = CalculateDisplacement(fragment_height_map_texture_coordinate, material_texture_coordinate);

	fragmentWorldPosition.y += mix(-0.5f, 0.5f, displacement);

	gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);
	
}