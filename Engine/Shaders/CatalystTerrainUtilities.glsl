#if !defined(CATALYST_TERRAIN_UTILITIES)
#define CATALYST_TERRAIN_UTILITIES

//Constants.
#define STRENGTHEN_DISPLACEMENT(X) (X * X * X)
#define TERRAIN_MATERIAL_COORDINATE_SCALE (0.25f)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    vec3 albedo;
    vec3 normal_map;
    vec4 material_properties;
};

/*
*   Calculates the terrain material.
*/
TerrainMaterial CalculateTerrainMaterial(vec2 height_map_texture_coordinate, vec2 material_texture_coordinate)
{
    //Retrieve the 4 materials to blend between.
    vec4 index_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_INDEX_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

    Material material_1 = GLOBAL_MATERIALS[int(index_map[0] * 255.0f)];
    Material material_2 = GLOBAL_MATERIALS[int(index_map[1] * 255.0f)];
    Material material_3 = GLOBAL_MATERIALS[int(index_map[2] * 255.0f)];
    Material material_4 = GLOBAL_MATERIALS[int(index_map[3] * 255.0f)];

    //Retrieve the 4 displacement values.
    float displacement_1 = texture(sampler2D(GLOBAL_TEXTURES[material_1._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
    float displacement_2 = texture(sampler2D(GLOBAL_TEXTURES[material_2._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
    float displacement_3 = texture(sampler2D(GLOBAL_TEXTURES[material_3._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
    float displacement_4 = texture(sampler2D(GLOBAL_TEXTURES[material_4._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;

    //Retrieve the blend map.
    vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_BLEND_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

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

    //Blend the material.
    TerrainMaterial material;

    material.albedo =   texture(sampler2D(GLOBAL_TEXTURES[material_1._AlbedoThickness], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[0]
                        + texture(sampler2D(GLOBAL_TEXTURES[material_2._AlbedoThickness], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[1]
                        + texture(sampler2D(GLOBAL_TEXTURES[material_3._AlbedoThickness], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[2]
                        + texture(sampler2D(GLOBAL_TEXTURES[material_4._AlbedoThickness], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[3];

    material.normal_map =   (texture(sampler2D(GLOBAL_TEXTURES[material_1._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[0]
                            + (texture(sampler2D(GLOBAL_TEXTURES[material_2._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[1]
                            + (texture(sampler2D(GLOBAL_TEXTURES[material_3._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[2]
                            + (texture(sampler2D(GLOBAL_TEXTURES[material_4._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[3];

    material.material_properties =  texture(sampler2D(GLOBAL_TEXTURES[material_1._MaterialProperties], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[0]
                                    + texture(sampler2D(GLOBAL_TEXTURES[material_2._MaterialProperties], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[1]
                                    + texture(sampler2D(GLOBAL_TEXTURES[material_3._MaterialProperties], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[2]
                                    + texture(sampler2D(GLOBAL_TEXTURES[material_4._MaterialProperties], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[3];

    return material;
}

/*
*   Blend two terrain materials.
*/
TerrainMaterial BlendTerrainMaterial(TerrainMaterial first, TerrainMaterial second, float alpha)
{
    TerrainMaterial output_material;

    output_material.albedo = mix(first.albedo, second.albedo, alpha);
    output_material.normal_map = mix(first.normal_map, second.normal_map, alpha);
    output_material.material_properties = mix(first.material_properties, second.material_properties, alpha);

    return output_material;
}

#endif