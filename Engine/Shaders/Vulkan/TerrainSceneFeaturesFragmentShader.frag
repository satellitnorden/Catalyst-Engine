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
    normal = pow(normal, vec3(2.0f));

    //Normalize the normal.
    float inverse_normal_sum = 1.0f / (normal.x + normal.y + normal.z);

    normal.x = normal.x * inverse_normal_sum;
    normal.y = normal.y * inverse_normal_sum;
    normal.z = normal.z * inverse_normal_sum;

    if (normal.x > normal.y + normal.z)
    {
        return world_position.yz;
    }

    else if (normal.y > normal.z)
    {
        return world_position.xz;
    }

    else
    {
        return world_position.xy;
    }


    /*
    //Sample the noise.
    float noise = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY))[0];

    if (noise <= normal.x)
    {
        return world_position.yz * 0.125f;
    }

    else if (noise <= (normal.x + normal.y))
    {
        return world_position.xz * 0.125f;
    }

    else
    {
        return world_position.xy * 0.125f;
    }
    */
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
vec4 SampleTerrainMaterial(int index, vec2 texture_coordinate)
{
    return texture(sampler2D(GLOBAL_TEXTURES[index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), texture_coordinate);
}

/*
*   Retrieves a single terrain material.
*/
void RetrieveSingleTerrainMaterial(int index, vec2 texture_coordinate, out TerrainMaterial material)
{
    material.albedo = SampleTerrainMaterial(GLOBAL_MATERIALS[index].albedo_texture_index, texture_coordinate).rgb;
    material.normal_and_height_map = SampleTerrainMaterial(GLOBAL_MATERIALS[index].normal_map_texture_index, texture_coordinate);
    material.normal_and_height_map.xyz = material.normal_and_height_map.xyz * 2.0f - 1.0f;
    material.material_properties = SampleTerrainMaterial(GLOBAL_MATERIALS[index].material_properties_texture_index, texture_coordinate);
}

/*
*   Bends two terrain materials.
*/
void BlendTerrainMaterials(TerrainMaterial first, TerrainMaterial second, float alpha, out TerrainMaterial material)
{
    //Calculate the alphas.
    float first_alpha = (1.0f - alpha) * pow(first.normal_and_height_map.w, MATERIAL_HEIGHT_BLEND_STRENGTH);
    float second_alpha = alpha * pow(second.normal_and_height_map.w, MATERIAL_HEIGHT_BLEND_STRENGTH);

    //And renormalize it!
    float inverse_alpha_sum = 1.0f / (first_alpha + second_alpha);

    first_alpha *= inverse_alpha_sum;
    second_alpha *= inverse_alpha_sum;

    material.albedo = first.albedo * first_alpha + second.albedo * second_alpha;
    material.normal_and_height_map = first.normal_and_height_map * first_alpha + second.normal_and_height_map * second_alpha;
    material.normal_and_height_map.xyz = normalize(material.normal_and_height_map.xyz);
    material.material_properties = first.material_properties * first_alpha + second.material_properties * second_alpha;
}

#define TRILINEAR (true)

/*
*   Retrieves the terrain material.
*/
void RetrieveTerrainMaterial(vec2 texture_coordinate, out TerrainMaterial material)
{
#if TRILINEAR
    //Calculate the horizontal and vertical alpha.
    float horizontal_alpha = fract(fragmentTextureCoordinate.x * terrain_texture_resolution);
    float vertical_alpha = fract(fragmentTextureCoordinate.y * terrain_texture_resolution);

    //Determine what quadrant this pixel is in.
    if (horizontal_alpha < 0.5f && vertical_alpha < 0.5f)
    {
        //Mutate the alphas to fit this quadrant.
        horizontal_alpha = horizontal_alpha + 0.5f;
        vertical_alpha = vertical_alpha + 0.5f;

        //Calculate the texture coordinates.
        vec2 first_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(-1.0f, 0.0f);
        vec2 second_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 0.0f);
        vec2 third_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(-1.0f, -1.0f);
        vec2 fourth_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, -1.0f);

        //Retrieve the terrain material indices.
        int first_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), first_texture_coordinate).w * 255.0f);
        int second_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), second_texture_coordinate).w * 255.0f);
        int third_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), third_texture_coordinate).w * 255.0f);
        int fourth_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fourth_texture_coordinate).w * 255.0f);


        //Retrieve the terrain materials.
        TerrainMaterial first_material;
        RetrieveSingleTerrainMaterial(first_terrain_material_index, texture_coordinate, first_material);
        TerrainMaterial second_material;
        RetrieveSingleTerrainMaterial(second_terrain_material_index, texture_coordinate, second_material);
        TerrainMaterial third_material;
        RetrieveSingleTerrainMaterial(third_terrain_material_index, texture_coordinate, third_material);
        TerrainMaterial fourth_material;
        RetrieveSingleTerrainMaterial(fourth_terrain_material_index, texture_coordinate, fourth_material);

        //Blend the terrain materials.
        TerrainMaterial blend_1;
        BlendTerrainMaterials(first_material, second_material, horizontal_alpha, blend_1);
        TerrainMaterial blend_2;
        BlendTerrainMaterials(third_material, fourth_material, horizontal_alpha, blend_2);

        BlendTerrainMaterials(blend_1, blend_2, vertical_alpha, material);
    }

    else if (horizontal_alpha < 0.5f && vertical_alpha >= 0.5f)
    {
        //Mutate the alphas to fit this quadrant.
        horizontal_alpha = horizontal_alpha + 0.5f;
        vertical_alpha = vertical_alpha - 0.5f;

        //Calculate the texture coordinates.
        vec2 first_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(-1.0f, 1.0f);
        vec2 second_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 1.0f);
        vec2 third_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(-1.0f, 0.0f);
        vec2 fourth_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 0.0f);

        //Retrieve the terrain material indices.
        int first_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), first_texture_coordinate).w * 255.0f);
        int second_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), second_texture_coordinate).w * 255.0f);
        int third_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), third_texture_coordinate).w * 255.0f);
        int fourth_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fourth_texture_coordinate).w * 255.0f);


        //Retrieve the terrain materials.
        TerrainMaterial first_material;
        RetrieveSingleTerrainMaterial(first_terrain_material_index, texture_coordinate, first_material);
        TerrainMaterial second_material;
        RetrieveSingleTerrainMaterial(second_terrain_material_index, texture_coordinate, second_material);
        TerrainMaterial third_material;
        RetrieveSingleTerrainMaterial(third_terrain_material_index, texture_coordinate, third_material);
        TerrainMaterial fourth_material;
        RetrieveSingleTerrainMaterial(fourth_terrain_material_index, texture_coordinate, fourth_material);

        //Blend the terrain materials.
        TerrainMaterial blend_1;
        BlendTerrainMaterials(first_material, second_material, horizontal_alpha, blend_1);
        TerrainMaterial blend_2;
        BlendTerrainMaterials(third_material, fourth_material, horizontal_alpha, blend_2);

        BlendTerrainMaterials(blend_1, blend_2, vertical_alpha, material); 

    }

    else if (horizontal_alpha >= 0.5f && vertical_alpha < 0.5f)
    {
        //Mutate the alphas to fit this quadrant.
        horizontal_alpha = horizontal_alpha - 0.5f;
        vertical_alpha = vertical_alpha + 0.5f;

        //Calculate the texture coordinates.
        vec2 first_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 0.0f);
        vec2 second_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 0.0f);
        vec2 third_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, -1.0f);
        vec2 fourth_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, -1.0f);

        //Retrieve the terrain material indices.
        int first_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), first_texture_coordinate).w * 255.0f);
        int second_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), second_texture_coordinate).w * 255.0f);
        int third_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), third_texture_coordinate).w * 255.0f);
        int fourth_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fourth_texture_coordinate).w * 255.0f);


        //Retrieve the terrain materials.
        TerrainMaterial first_material;
        RetrieveSingleTerrainMaterial(first_terrain_material_index, texture_coordinate, first_material);
        TerrainMaterial second_material;
        RetrieveSingleTerrainMaterial(second_terrain_material_index, texture_coordinate, second_material);
        TerrainMaterial third_material;
        RetrieveSingleTerrainMaterial(third_terrain_material_index, texture_coordinate, third_material);
        TerrainMaterial fourth_material;
        RetrieveSingleTerrainMaterial(fourth_terrain_material_index, texture_coordinate, fourth_material);

        //Blend the terrain materials.
        TerrainMaterial blend_1;
        BlendTerrainMaterials(first_material, second_material, horizontal_alpha, blend_1);
        TerrainMaterial blend_2;
        BlendTerrainMaterials(third_material, fourth_material, horizontal_alpha, blend_2);

        BlendTerrainMaterials(blend_1, blend_2, vertical_alpha, material); 
    }

    else /*if (horizontal_alpha >= 0.5f && vertical_alpha >= 0.5f)*/
    {
        //Mutate the alphas to fit this quadrant.
        horizontal_alpha = horizontal_alpha - 0.5f;
        vertical_alpha = vertical_alpha - 0.5f;

        //Calculate the texture coordinates.
        vec2 first_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 1.0f);
        vec2 second_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 1.0f);
        vec2 third_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 0.0f);
        vec2 fourth_texture_coordinate = fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 0.0f);

        //Retrieve the terrain material indices.
        int first_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), first_texture_coordinate).w * 255.0f);
        int second_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), second_texture_coordinate).w * 255.0f);
        int third_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), third_texture_coordinate).w * 255.0f);
        int fourth_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fourth_texture_coordinate).w * 255.0f);


        //Retrieve the terrain materials.
        TerrainMaterial first_material;
        RetrieveSingleTerrainMaterial(first_terrain_material_index, texture_coordinate, first_material);
        TerrainMaterial second_material;
        RetrieveSingleTerrainMaterial(second_terrain_material_index, texture_coordinate, second_material);
        TerrainMaterial third_material;
        RetrieveSingleTerrainMaterial(third_terrain_material_index, texture_coordinate, third_material);
        TerrainMaterial fourth_material;
        RetrieveSingleTerrainMaterial(fourth_terrain_material_index, texture_coordinate, fourth_material);

        //Blend the terrain materials.
        TerrainMaterial blend_1;
        BlendTerrainMaterials(first_material, second_material, horizontal_alpha, blend_1);
        TerrainMaterial blend_2;
        BlendTerrainMaterials(third_material, fourth_material, horizontal_alpha, blend_2);

        BlendTerrainMaterials(blend_1, blend_2, vertical_alpha, material); 
    }
#else
    //Retrieve the terrain material indices.
    int center_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).w * 255.0f);
    int right_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 0.0f)).w * 255.0f);
    int upper_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(0.0f, 1.0f)).w * 255.0f);
    int upper_right_terrain_material_index = int(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate + MATERIAL_TEXTURE_COORDINATE_OFFSET * vec2(1.0f, 1.0f)).w * 255.0f);


    //Retrieve the terrain materials.
    TerrainMaterial center_material;
    RetrieveSingleTerrainMaterial(center_terrain_material_index, texture_coordinate, center_material);
    TerrainMaterial right_material;
    RetrieveSingleTerrainMaterial(right_terrain_material_index, texture_coordinate, right_material);
    TerrainMaterial upper_material;
    RetrieveSingleTerrainMaterial(upper_terrain_material_index, texture_coordinate, upper_material);
    TerrainMaterial upper_right_material;
    RetrieveSingleTerrainMaterial(upper_right_terrain_material_index, texture_coordinate, upper_right_material);

    //Calculate the horizontal and vertical alpha.
    float horizontal_alpha = fract(fragmentTextureCoordinate.x * terrain_texture_resolution);
    float vertical_alpha = fract(fragmentTextureCoordinate.y * terrain_texture_resolution);

    //Blend the terrain materials.
    TerrainMaterial blend_1;
    BlendTerrainMaterials(center_material, right_material, horizontal_alpha, blend_1);
    TerrainMaterial blend_2;
    BlendTerrainMaterials(upper_material, upper_right_material, horizontal_alpha, blend_2);

    BlendTerrainMaterials(blend_1, blend_2, vertical_alpha, material);
#endif
}

void main()
{
    //Retrieve the terrain normal.
    vec3 terrain_normal = normalize(texture(sampler2D(GLOBAL_TEXTURES[normal_and_material_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f);

    //Calculate the texture coordinate.
    vec2 texture_coordinate = CalculateTextureCoordinate(fragmentWorldPosition, terrain_normal);

    //Retrieve the terrain material.
    TerrainMaterial material;
    RetrieveTerrainMaterial(texture_coordinate, material);

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
    sceneFeatures1 = vec4(material.albedo, 0.0f);
    sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, gl_FragCoord.z);
    sceneFeatures3 = material.material_properties;
}