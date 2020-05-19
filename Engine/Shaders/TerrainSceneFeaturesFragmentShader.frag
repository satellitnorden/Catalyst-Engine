//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystTerrainUtilities.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystTerrain.h"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 world_position;
    layout (offset = 8) float patch_size;
    layout (offset = 12) int borders;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragment_height_map_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
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

void CatalystShaderMain()
{
	//Calculate the surrounding heights.
#define OFFSET (1.0f / TERRAIN_MAP_RESOLUTION)

	float center_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate).x;
	float right_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(OFFSET, 0.0f)).x;
	float up_height = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_HEIGHT_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(0.0f, OFFSET)).x;

	//Calculate the tangent space matrix.
	mat3 tangent_space_matrix = CalculateTerrainTangentSpaceMatrix(center_height, right_height, up_height);

    //Calculate the material texture coordinate.
	vec2 material_texture_coordinate = fragmentWorldPosition.xz * TERRAIN_MATERIAL_COORDINATE_SCALE;

	//Calculate the material.
	TerrainMaterial first_material 	= CalculateTerrainMaterial(fragment_height_map_texture_coordinate, material_texture_coordinate);
	TerrainMaterial second_material = CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(0.0f, 1.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);
	TerrainMaterial third_material 	= CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(1.0f, 0.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);
	TerrainMaterial fourth_material = CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(1.0f, 1.0f) / TERRAIN_MAP_RESOLUTION, material_texture_coordinate);

	TerrainMaterial blend_1 = BlendTerrainMaterial(first_material, second_material, fract(fragment_height_map_texture_coordinate.y * TERRAIN_MAP_RESOLUTION));
	TerrainMaterial blend_2 = BlendTerrainMaterial(third_material, fourth_material, fract(fragment_height_map_texture_coordinate.y * TERRAIN_MAP_RESOLUTION));

	TerrainMaterial final_material = BlendTerrainMaterial(blend_1, blend_2, fract(fragment_height_map_texture_coordinate.x * TERRAIN_MAP_RESOLUTION));

	final_material.normal_map = normalize(final_material.normal_map);

	//Calculate the shading normal.
	vec3 shading_normal = normalize(tangent_space_matrix * final_material.normal_map);

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragmentWorldPosition) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragmentWorldPosition);

    //Write the fragments.
    sceneFeatures1 = vec4(final_material.albedo, 1.0f);
    sceneFeatures2 = vec4(shading_normal, gl_FragCoord.z);
    sceneFeatures3 = final_material.material_properties;
    scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}