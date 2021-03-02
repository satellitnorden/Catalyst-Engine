//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystTerrainUtilities.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystTerrain.h"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) vec2 WORLD_POSITION;
    layout (offset = 24) float PATCH_SIZE;
    layout (offset = 28) int BORDERS;
    layout (offset = 32) float VERTEX_BORDER_OFFSET_FIRST;
    layout (offset = 36) float VERTEX_BORDER_OFFSET_SECOND;
    layout (offset = 40) uint HEIGHT_MAP_TEXTURE_INDEX;
    layout (offset = 44) uint INDEX_MAP_TEXTURE_INDEX;
    layout (offset = 48) uint BLEND_MAP_TEXTURE_INDEX;
    layout (offset = 52) float HEIGHT_MAP_RESOLOLUTION_RECIPROCAL;
    layout (offset = 56) float METER_PER_HEIGHT_MAP_TEXEL;
    layout (offset = 60) float MATERIAL_MAPS_RESOLOLUTION;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec2 fragment_height_map_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;
layout (location = 3) out vec4 scene_features_4;

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenWORLD_TO_CLIP_MATRIX, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenWORLD_TO_CLIP_MATRIX * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

/*
*	Samples the height at the given coordinate.
*/
float SampleHeight(vec2 coordinate)
{
	return texture(sampler2D(GLOBAL_TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate).x;
}

void CatalystShaderMain()
{
	//Calculate the tangent space matrix.
	vec2 left_coordinate = fragment_height_map_texture_coordinate + vec2(-HEIGHT_MAP_RESOLOLUTION_RECIPROCAL, 0.0f);
	vec2 right_coordinate = fragment_height_map_texture_coordinate + vec2(HEIGHT_MAP_RESOLOLUTION_RECIPROCAL, 0.0f);
	vec2 down_coordinate = fragment_height_map_texture_coordinate + vec2(0.0f, -HEIGHT_MAP_RESOLOLUTION_RECIPROCAL);
	vec2 up_coordinate = fragment_height_map_texture_coordinate + vec2(0.0f, HEIGHT_MAP_RESOLOLUTION_RECIPROCAL);

	float left_height = SampleHeight(left_coordinate);
	float right_height = SampleHeight(right_coordinate);
	float down_height = SampleHeight(down_coordinate);
	float up_height = SampleHeight(up_coordinate);

	float left_overflow = (abs(clamp(left_coordinate.x, -HEIGHT_MAP_RESOLOLUTION_RECIPROCAL, 0.0f)) * 33.0f);
	float right_overflow = (clamp(right_coordinate.x - 1.0f, 0.0f, HEIGHT_MAP_RESOLOLUTION_RECIPROCAL) * 33.0f);
	float down_overflow = (abs(clamp(down_coordinate.y, -HEIGHT_MAP_RESOLOLUTION_RECIPROCAL, 0.0f)) * 33.0f);
	float up_overflow = (clamp(up_coordinate.y - 1.0f, 0.0f, HEIGHT_MAP_RESOLOLUTION_RECIPROCAL) * 33.0f);

	vec3 tangent = normalize(vec3(mix(METER_PER_HEIGHT_MAP_TEXEL, 0.0f, right_overflow), right_height, 0.0f) - vec3(mix(-METER_PER_HEIGHT_MAP_TEXEL, 0.0f, left_overflow), left_height, 0.0f));
	vec3 bitangent = normalize(vec3(0.0f, up_height, mix(METER_PER_HEIGHT_MAP_TEXEL, 0.0f, down_overflow)) - vec3(0.0f, down_height, mix(-METER_PER_HEIGHT_MAP_TEXEL, 0.0f, up_overflow)));
	vec3 normal = normalize(cross(tangent, bitangent));

	normal = dot(normal, vec3(0.0f, 1.0f, 0.0f)) >= 0.0f ? normal : -normal;

	mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

    //Calculate the material texture coordinate.
	vec2 material_texture_coordinate = fragment_world_position.xz * TERRAIN_MATERIAL_COORDINATE_SCALE;

	//Calculate the material.
	TerrainMaterial first_material 	= CalculateTerrainMaterial(fragment_height_map_texture_coordinate, material_texture_coordinate, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);
	TerrainMaterial second_material = CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(0.0f, 1.0f) / MATERIAL_MAPS_RESOLOLUTION, material_texture_coordinate, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);
	TerrainMaterial third_material 	= CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(1.0f, 0.0f) / MATERIAL_MAPS_RESOLOLUTION, material_texture_coordinate, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);
	TerrainMaterial fourth_material = CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(1.0f, 1.0f) / MATERIAL_MAPS_RESOLOLUTION, material_texture_coordinate, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);

	TerrainMaterial blend_1 = BlendTerrainMaterial(first_material, second_material, fract(fragment_height_map_texture_coordinate.y * MATERIAL_MAPS_RESOLOLUTION));
	TerrainMaterial blend_2 = BlendTerrainMaterial(third_material, fourth_material, fract(fragment_height_map_texture_coordinate.y * MATERIAL_MAPS_RESOLOLUTION));

	TerrainMaterial final_material = BlendTerrainMaterial(blend_1, blend_2, fract(fragment_height_map_texture_coordinate.x * MATERIAL_MAPS_RESOLOLUTION));

	final_material.normal_map = normalize(final_material.normal_map);

	//Calculate the shading normal.
	vec3 shading_normal = normalize(tangent_space_matrix * final_material.normal_map);

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position + WORLD_GRID_DELTA) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position + WORLD_GRID_DELTA);

    //Write the fragments.
    scene_features_1 = vec4(final_material.albedo, 1.0f);
    scene_features_2 = vec4(shading_normal, gl_FragCoord.z);
    scene_features_3 = final_material.material_properties;
    scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}