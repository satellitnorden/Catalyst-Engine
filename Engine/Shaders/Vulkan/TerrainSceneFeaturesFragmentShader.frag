//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"

//Constants.
#define MATERIAL_TEXTURE_COORDINATE_OFFSET (vec2(inverse_terrain_texture_resolution, inverse_terrain_texture_resolution))
#define MATERIAL_HEIGHT_BLEND_STRENGTH (2.0f)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    vec3 albedo;
    vec4 normal_and_height_map;
    vec4 material_properties;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 20) float terrain_texture_resolution;
    layout (offset = 24) float inverse_terrain_texture_resolution;
    layout (offset = 28) int normal_and_material_texture_index;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;

/*
*   Calculates the texture coordinate.
*/
vec2 CalculateTextureCoordinate(vec3 world_position, vec3 normal)
{
    //Only use the absolute values of the normal for this calculation.
    normal = abs(normal);

    //Mutate the normal a bit so that the noise is less noticable on more flat surfaces.
    normal *= normal;

    //Normalize the normal.
    float inverse_normal_sum = 1.0f / (normal.x + normal.y + normal.z);

    normal.x = normal.x * inverse_normal_sum;
    normal.y = normal.y * inverse_normal_sum;
    normal.z = normal.z * inverse_normal_sum;

    //Sample the noise.
    vec4 noise = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

    if (noise[0] <= normal.x)
    {
        return world_position.yz * 0.25f;
    }

    else if (noise[0] <= (normal.x + normal.y))
    {
        return world_position.xz * 0.25f;
    }

    else
    {
        return world_position.xy * 0.25f;
    }
}

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
    return  texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), world_position.yz * 0.125f) * normalized_normal.x
            + texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), world_position.xz * 0.125f) * normalized_normal.y
            + texture(sampler2D(globalTextures[index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), world_position.xy * 0.125f) * normalized_normal.z;
}

/*
*   Retrieves a single terrain material.
*/
void RetrieveSingleTerrainMaterial(int index, vec3 world_position, vec3 normalized_normal, out TerrainMaterial material)
{
    material.albedo = SampleTerrainMaterial(GLOBAL_MATERIALS[index].albedo_texture_index, world_position, normalized_normal).rgb;
    material.normal_and_height_map = SampleTerrainMaterial(GLOBAL_MATERIALS[index].normal_map_texture_index, world_position, normalized_normal);
    material.normal_and_height_map.xyz = material.normal_and_height_map.xyz * 2.0f - 1.0f;
    material.material_properties = SampleTerrainMaterial(GLOBAL_MATERIALS[index].material_properties_texture_index, world_position, normalized_normal);
}

/*
*   Bends two terrain materials.
*/
void BlendTerrainMaterials(TerrainMaterial first, TerrainMaterial second, float alpha, out TerrainMaterial material)
{
    //Calculate the alphas.
    float first_alpha = (1.0f - alpha) * first.normal_and_height_map.w;
    float second_alpha = alpha * second.normal_and_height_map.w;

    //Strengthen the height influence on the alphas.
    first_alpha = pow(first_alpha, MATERIAL_HEIGHT_BLEND_STRENGTH);
    second_alpha = pow(second_alpha, MATERIAL_HEIGHT_BLEND_STRENGTH);

    //And renormalize it!
    float inverse_alpha_sum = 1.0f / (first_alpha + second_alpha);

    first_alpha *= inverse_alpha_sum;
    second_alpha *= inverse_alpha_sum;

    material.albedo = first.albedo * first_alpha + second.albedo * second_alpha;
    material.normal_and_height_map = first.normal_and_height_map * first_alpha + second.normal_and_height_map * second_alpha;
    material.normal_and_height_map.xyz = normalize(material.normal_and_height_map.xyz);
    material.material_properties = first.material_properties * first_alpha + second.material_properties * second_alpha;
}

/*
*   Retrieves the terrain material.
*/
void RetrieveTerrainMaterial(vec3 terrain_normal, out TerrainMaterial material)
{
    //Retrieve the terrain material indices.
    int center_terrain_material_index = int(texture(sampler2D(globalTextures[normal_and_material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).w * 255.0f);
    int right_terrain_material_index = int(texture(sampler2D(globalTextures[normal_and_material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 0.0f)).w * 255.0f);
    int upper_terrain_material_index = int(texture(sampler2D(globalTextures[normal_and_material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 1.0f)).w * 255.0f);
    int upper_right_terrain_material_index = int(texture(sampler2D(globalTextures[normal_and_material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 1.0f)).w * 255.0f);

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
    vec3 terrain_normal = normalize(texture(sampler2D(globalTextures[normal_and_material_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f);

    //Retrieve the terrain material.
    TerrainMaterial material;
    RetrieveTerrainMaterial(terrain_normal, material);

    //Calculate the tangent space matrix.
    vec3 normal = terrain_normal;
    vec3 tangent = cross(normal, vec3(1.0f, 0.0f, 0.0f));
    vec3 bitangent = cross(normal, tangent);

    mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

    //Calculate the shading normal.
    vec3 shading_normal = normalize(tangent_space_matrix * material.normal_and_height_map.xyz);

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragmentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentWorldPosition);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(material.albedo, vec3(2.2f)), 0.0f);
    sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, length(fragmentWorldPosition - perceiverWorldPosition));
    sceneFeatures3 = material.material_properties;
}