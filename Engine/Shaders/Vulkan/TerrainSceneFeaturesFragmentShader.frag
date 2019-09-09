//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 20) int normalTextureIndex;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 sceneFeatures4;
layout (location = 4) out vec4 velocity;

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenViewMatrix, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenViewMatrix * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

void main()
{
    //Retrieve the terrain normal.
    vec3 terrain_normal = normalize(texture(sampler2D(globalTextures[normalTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f);

    //calculate the material texture coordinate.
    vec2 materialTextureCoordinate = fragmentWorldPosition.xz;

    //Retrieve the albedo, normal map and material properties.
    vec3 albedo;
    vec3 normal_map;
    vec4 material_properties;

    if (dot(terrain_normal, vec3(0.0f, 1.0f, 0.0f)) >= 0.75f)
    {
        albedo = texture(sampler2D(globalTextures[global_materials[0].albedo_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate).rgb;
        normal_map = texture(sampler2D(globalTextures[global_materials[0].normal_map_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate).xyz * 2.0f - 1.0f;
        material_properties = texture(sampler2D(globalTextures[global_materials[0].material_properties_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate);
    }
    
    else
    {
        albedo = texture(sampler2D(globalTextures[global_materials[1].albedo_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate).rgb;
        normal_map = texture(sampler2D(globalTextures[global_materials[1].normal_map_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate).xyz * 2.0f - 1.0f;
        material_properties = texture(sampler2D(globalTextures[global_materials[1].material_properties_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate);
    }

    //Calculate the tangent space matrix.
    vec3 normal = terrain_normal;
    vec3 tangent = cross(normal, vec3(1.0f, 0.0f, 0.0f));
    vec3 bitangent = cross(normal, tangent);

    mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

    //Calculate the shading normal.
    vec3 shading_normal = normalize(tangent_space_matrix * normal_map);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(albedo, vec3(2.2f)), 0.0f);
    sceneFeatures2 = vec4(terrain_normal, length(fragmentWorldPosition - perceiverWorldPosition));
    sceneFeatures3 = vec4(shading_normal, 0.0f);
    sceneFeatures4 = material_properties;
    velocity = vec4(CalculateScreenCoordinate(viewMatrix, fragmentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentWorldPosition), 0.0f, 0.0f);
}