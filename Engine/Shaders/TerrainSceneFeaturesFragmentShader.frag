//Includes.
#include "CatalystTerrainUtilities.glsl"
//#include "..\Include\Rendering\Native\Shader\CatalystTerrain.h"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_POSITION;
    layout (offset = 16) float PATCH_SIZE;
    layout (offset = 20) uint BORDERS;
    layout (offset = 24) float VERTEX_BORDER_OFFSET_FIRST;
    layout (offset = 28) float VERTEX_BORDER_OFFSET_SECOND;
    layout (offset = 32) uint HEIGHT_MAP_TEXTURE_INDEX;
    layout (offset = 36) uint NORMAL_MAP_TEXTURE_INDEX;
    layout (offset = 40) uint INDEX_MAP_TEXTURE_INDEX;
    layout (offset = 44) uint BLEND_MAP_TEXTURE_INDEX;
    layout (offset = 48) float MAP_RESOLUTION;
    layout (offset = 52) float MAP_RESOLUTION_RECIPROCAL;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec2 fragment_height_map_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;
layout (location = 3) out vec4 scene_features_4;

void CatalystShaderMain()
{
	//Retrieve the normal.
	vec3 normal = texture(sampler2D(GLOBAL_TEXTURES[NORMAL_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate).xyz;
	normal = normal * 2.0f - 1.0f;

	//Calculate the tangent space matrix.
	vec3 tangent = cross(normal, vec3(0.0f, 0.0f, 1.0f));
	vec3 bitangent = cross(tangent, normal);

	mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

	//Calculate the material.
	TerrainMaterial first_material 	= CalculateTerrainMaterial(fragment_height_map_texture_coordinate, fragment_world_position.xz, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);
	TerrainMaterial second_material = CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL, fragment_world_position.xz, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);
	TerrainMaterial third_material 	= CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL, fragment_world_position.xz, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);
	TerrainMaterial fourth_material = CalculateTerrainMaterial(fragment_height_map_texture_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL, fragment_world_position.xz, INDEX_MAP_TEXTURE_INDEX, BLEND_MAP_TEXTURE_INDEX);

	TerrainMaterial blend_1 = BlendTerrainMaterial(first_material, second_material, fract(fragment_height_map_texture_coordinate.y * MAP_RESOLUTION));
	TerrainMaterial blend_2 = BlendTerrainMaterial(third_material, fourth_material, fract(fragment_height_map_texture_coordinate.y * MAP_RESOLUTION));

	TerrainMaterial final_material = BlendTerrainMaterial(blend_1, blend_2, fract(fragment_height_map_texture_coordinate.x * MAP_RESOLUTION));

	final_material.normal_map = normalize(final_material.normal_map);

	//Calculate the shading normal.
	vec3 shading_normal = normalize(tangent_space_matrix * final_material.normal_map);

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position);

    //Write the fragments.
    scene_features_1 = vec4(final_material.albedo, 1.0f);
    scene_features_2 = vec4(shading_normal, gl_FragCoord.z);
    scene_features_3 = final_material.material_properties;
    scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}