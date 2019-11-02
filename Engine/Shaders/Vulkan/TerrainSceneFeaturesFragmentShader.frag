//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"

//Constants.
#define MATERIAL_TEXTURE_COORDINATE_OFFSET (vec2(inverse_terrain_texture_resolution, inverse_terrain_texture_resolution))

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    vec3 albedo;
    vec3 normal_map;
    vec4 material_properties;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 20) float terrain_texture_resolution;
    layout (offset = 24) float inverse_terrain_texture_resolution;
    layout (offset = 28) int normal_texture_index;
    layout (offset = 32) int material_texture_index;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures4;
layout (location = 3) out vec4 velocity;

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenViewMatrix, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenViewMatrix * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

/*
*   Samples the terrain material at the given world position.
*/
vec4 SampleTerrainMaterial(int index, vec3 world_position, vec3 normalized_normal)
{
    return  texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), world_position.yz * 0.5f) * normalized_normal.x
            + texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), world_position.xz * 0.5f) * normalized_normal.y
            + texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), world_position.xy * 0.5f) * normalized_normal.z;

    /*
    vec2 coordinate =   world_position.yz * float(normalized_normal.x > normalized_normal.y && normalized_normal.x > normalized_normal.z)
                        + world_position.xz * float(normalized_normal.y > normalized_normal.x && normalized_normal.y > normalized_normal.z)
                        + world_position.xy * float(normalized_normal.z > normalized_normal.x && normalized_normal.z > normalized_normal.y);

    coordinate *= 0.5f;

    return texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
    */
}

/*
*   Retrieves a single terrain material.
*/
void RetrieveSingleTerrainMaterial(int index, vec3 world_position, vec3 normalized_normal, out TerrainMaterial material)
{
    material.albedo = SampleTerrainMaterial(global_materials[index].albedo_texture_index, world_position, normalized_normal).rgb;
    material.normal_map = SampleTerrainMaterial(global_materials[index].normal_map_texture_index, world_position, normalized_normal).xyz * 2.0f - 1.0f;
    material.material_properties = SampleTerrainMaterial(global_materials[index].material_properties_texture_index, world_position, normalized_normal);
}

/*
*   Bends two terrain materials.
*/
void BlendTerrainMaterials(TerrainMaterial first, TerrainMaterial second, float alpha, out TerrainMaterial material)
{
    /*
    *   Modify the alpha a bit, to take int account the "height" of the material.
    *   The "height" is approximated from the normal map.
    */
    float first_height = max(dot(first.normal_map, vec3(0.0f, 0.0f, 1.0f)), 0.0f);
    float second_height = max(dot(second.normal_map, vec3(0.0f, 0.0f, 1.0f)), 0.0f);

    float first_alpha = (1.0f - alpha) * first_height;
    float second_alpha = alpha * second_height;

    //And renormalize it!
    float inverse_alpha_sum = 1.0f / (first_alpha + second_alpha);

    first_alpha *= inverse_alpha_sum;
    second_alpha *= inverse_alpha_sum;

    material.albedo = first.albedo * first_alpha + second.albedo * second_alpha;
    material.normal_map = normalize(first.normal_map * first_alpha + second.normal_map * second_alpha);
    material.material_properties = first.material_properties * first_alpha + second.material_properties * second_alpha;
}

/*
*   Retrieves the terrain material.
*/
void RetrieveTerrainMaterial(vec3 terrain_normal, out TerrainMaterial material)
{
    //Retrieve the terrain material indices.
    int center_terrain_material_index = int(texture(sampler2D(globalTextures[material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).x * 255.0f);
    int right_terrain_material_index = int(texture(sampler2D(globalTextures[material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 0.0f)).x * 255.0f);
    int upper_terrain_material_index = int(texture(sampler2D(globalTextures[material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 1.0f)).x * 255.0f);
    int upper_right_terrain_material_index = int(texture(sampler2D(globalTextures[material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 1.0f)).x * 255.0f);

    //Calculate the normalized normal.
    terrain_normal = abs(terrain_normal);
    vec3 normalized_normal;

    float terrain_normal_sum = 1.0f / (terrain_normal.x + terrain_normal.y + terrain_normal.z);

    normalized_normal.x = terrain_normal.x * terrain_normal_sum;
    normalized_normal.y = terrain_normal.y * terrain_normal_sum;
    normalized_normal.z = terrain_normal.z * terrain_normal_sum;

    //Retrieve the terrain materials.
    TerrainMaterial center_material;
    RetrieveSingleTerrainMaterial(center_terrain_material_index, fragmentWorldPosition, normalized_normal, center_material);
    TerrainMaterial right_material;
    RetrieveSingleTerrainMaterial(right_terrain_material_index, fragmentWorldPosition, normalized_normal, right_material);
    TerrainMaterial upper_material;
    RetrieveSingleTerrainMaterial(upper_terrain_material_index, fragmentWorldPosition, normalized_normal, upper_material);
    TerrainMaterial upper_right_material;
    RetrieveSingleTerrainMaterial(upper_right_terrain_material_index, fragmentWorldPosition, normalized_normal, upper_right_material);

    //Calculate the horizontal and vertical alpha.
    float horizontal_alpha = fract(fragmentTextureCoordinate.x * terrain_texture_resolution);
    float vertical_alpha = fract(fragmentTextureCoordinate.y * terrain_texture_resolution);

    //Blend the terrain materials.
    TerrainMaterial blend_1;
    BlendTerrainMaterials(center_material, right_material, horizontal_alpha, blend_1);
    TerrainMaterial blend_2;
    BlendTerrainMaterials(upper_material, upper_right_material, horizontal_alpha, blend_2);

    BlendTerrainMaterials(blend_1, blend_2, vertical_alpha, material);
}

void main()
{
    //Retrieve the terrain normal.
    vec3 terrain_normal = normalize(texture(sampler2D(globalTextures[normal_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f);

    //Retrieve the terrain material.
    TerrainMaterial material;
    RetrieveTerrainMaterial(terrain_normal, material);

    //Calculate the tangent space matrix.
    vec3 normal = terrain_normal;
    vec3 tangent = cross(normal, vec3(1.0f, 0.0f, 0.0f));
    vec3 bitangent = cross(normal, tangent);

    mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

    //Calculate the shading normal.
    vec3 shading_normal = normalize(tangent_space_matrix * material.normal_map);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(material.albedo, vec3(2.2f)), 0.0f);
    sceneFeatures2 = vec4(PackNormal(terrain_normal), PackNormal(shading_normal), 0.0f, length(fragmentWorldPosition - perceiverWorldPosition));
    sceneFeatures4 = material.material_properties;
    velocity = vec4(CalculateScreenCoordinate(viewMatrix, fragmentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentWorldPosition), 0.0f, 0.0f);
}