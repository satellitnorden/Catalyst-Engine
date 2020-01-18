//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 world_position;
    layout (offset = 8) float patch_size;
    layout (offset = 12) int borders;
    layout (offset = 16) int height_map_texture_index;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragment_height_map_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;

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
*	Calculates the terrain normal.
*/
vec3 CalculateTerrainNormal()
{
#define OFFSET (1.0f / 2048.0f)

	float left = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(-OFFSET, 0.0f)).x;
	float right = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(OFFSET, 0.0f)).x;
	float down = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(0.0f, -OFFSET)).x;
	float up = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(0.0f, OFFSET)).x;

	return normalize(vec3(left - right, 2.0f, down - up));
}

void main()
{
	//Calculate the terrain normal.
	vec3 terrain_normal = CalculateTerrainNormal();

	//Calculate the tangent space matrix.
	mat3 tangent_space_matrix = CalculateTangentSpaceMatrix(terrain_normal);

    //Calculate the material texture coordinate.
	vec2 material_texture_coordinate = fragmentWorldPosition.xz * 0.25f;

   	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[1];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, material_texture_coordinate);

	//Sample the material properties.
	vec4 material_properties = RetrieveMaterialProperties(material, material_texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal;

	if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
	{
		shading_normal = tangent_space_matrix[2];
	}

	else
	{
		//Sample the normal map.
		vec3 normal_map = texture(sampler2D(GLOBAL_TEXTURES[material.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz;
		shading_normal = normal_map * 2.0f - 1.0f;
		shading_normal = tangent_space_matrix * shading_normal;
		shading_normal = normalize(shading_normal);
	}

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragmentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentWorldPosition);

    //Write the fragments.
    sceneFeatures1 = vec4(albedo, float(1) / 255.0f);
    sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, gl_FragCoord.z);
    sceneFeatures3 = material_properties;
}