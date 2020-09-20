#if !defined(CATALYST_TERRAIN_UTILITIES)
#define CATALYST_TERRAIN_UTILITIES

//Includes.
#include "CatalystMaterialCore.glsl"

//Constants.
#define STRENGTHEN_DISPLACEMENT(X) (X * X * X)
#define TERRAIN_MATERIAL_COORDINATE_SCALE (0.25f)
#define TERRAIN_MINIMUM_DISPLACEMENT (0.001f)

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

    Material material_1 = GLOBAL_MATERIALS[uint(index_map[0] * 255.0f)];
    Material material_2 = GLOBAL_MATERIALS[uint(index_map[1] * 255.0f)];
    Material material_3 = GLOBAL_MATERIALS[uint(index_map[2] * 255.0f)];
    Material material_4 = GLOBAL_MATERIALS[uint(index_map[3] * 255.0f)];

    //Evalute the materials.
    vec4 material_1_albedo_thickness;
    vec4 material_2_albedo_thickness;
    vec4 material_3_albedo_thickness;
    vec4 material_4_albedo_thickness;

    vec4 material_1_normal_map_displacement;
    vec4 material_2_normal_map_displacement;
    vec4 material_3_normal_map_displacement;
    vec4 material_4_normal_map_displacement;

    vec4 material_1_material_properties;
    vec4 material_2_material_properties;
    vec4 material_3_material_properties;
    vec4 material_4_material_properties;

    vec4 material_1_opacity;
    vec4 material_2_opacity;
    vec4 material_3_opacity;
    vec4 material_4_opacity;

    EvaluateMaterial(   material_1,
                        material_texture_coordinate,
                        material_1_albedo_thickness,
                        material_1_normal_map_displacement,
                        material_1_material_properties,
                        material_1_opacity);

    EvaluateMaterial(   material_2,
                        material_texture_coordinate,
                        material_2_albedo_thickness,
                        material_2_normal_map_displacement,
                        material_2_material_properties,
                        material_2_opacity);

    EvaluateMaterial(   material_3,
                        material_texture_coordinate,
                        material_3_albedo_thickness,
                        material_3_normal_map_displacement,
                        material_3_material_properties,
                        material_3_opacity);

    EvaluateMaterial(   material_4,
                        material_texture_coordinate,
                        material_4_albedo_thickness,
                        material_4_normal_map_displacement,
                        material_4_material_properties,
                        material_4_opacity);

    material_1_normal_map_displacement.w = max(material_1_normal_map_displacement.w, TERRAIN_MINIMUM_DISPLACEMENT);
    material_2_normal_map_displacement.w = max(material_2_normal_map_displacement.w, TERRAIN_MINIMUM_DISPLACEMENT);
    material_3_normal_map_displacement.w = max(material_3_normal_map_displacement.w, TERRAIN_MINIMUM_DISPLACEMENT);
    material_4_normal_map_displacement.w = max(material_4_normal_map_displacement.w, TERRAIN_MINIMUM_DISPLACEMENT);

    //Retrieve the blend map.
    vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[TERRAIN_BLEND_MAP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

    //Alter the blend values based on the displacement values.
    blend_map[0] *= STRENGTHEN_DISPLACEMENT(material_1_normal_map_displacement.w);
    blend_map[1] *= STRENGTHEN_DISPLACEMENT(material_2_normal_map_displacement.w);
    blend_map[2] *= STRENGTHEN_DISPLACEMENT(material_3_normal_map_displacement.w);
    blend_map[3] *= STRENGTHEN_DISPLACEMENT(material_4_normal_map_displacement.w);

    //Renormalize the blend map.
    float inverse_sum = 1.0f / (blend_map[0] + blend_map[1] + blend_map[2] + blend_map[3]);

    blend_map[0] *= inverse_sum;
    blend_map[1] *= inverse_sum;
    blend_map[2] *= inverse_sum;
    blend_map[3] *= inverse_sum;

    //Blend the material.
    TerrainMaterial material;

    material.albedo =   material_1_albedo_thickness.rgb * blend_map[0]
                        + material_2_albedo_thickness.rgb * blend_map[1]
                        + material_3_albedo_thickness.rgb * blend_map[2]
                        + material_4_albedo_thickness.rgb * blend_map[3];

    material.normal_map =   (material_1_normal_map_displacement.xyz * 2.0f - 1.0f) * blend_map[0]
                            + (material_2_normal_map_displacement.xyz * 2.0f - 1.0f) * blend_map[1]
                            + (material_3_normal_map_displacement.xyz * 2.0f - 1.0f) * blend_map[2]
                            + (material_4_normal_map_displacement.xyz * 2.0f - 1.0f) * blend_map[3];

    material.material_properties =  material_1_material_properties * blend_map[0]
                                    + material_2_material_properties * blend_map[1]
                                    + material_3_material_properties * blend_map[2]
                                    + material_4_material_properties * blend_map[3];

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