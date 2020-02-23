//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"

//Constants.
#define STRENGTHEN_DISPLACEMENT(X) (X * X)

/*
*	Terrain material struct definition.
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
    layout (offset = 0) vec2 world_position;
    layout (offset = 8) float patch_size;
    layout (offset = 12) int borders;
    layout (offset = 16) int height_map_texture_index;
    layout (offset = 20) int index_map_texture_index;
    layout (offset = 24) int blend_map_texture_index;
    layout (offset = 28) float map_resolution;
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
*	Calculates the terrain tangent space matrix.
*/
mat3 CalculateTerrainTangentSpaceMatrix()
{
#define OFFSET (1.0f / map_resolution)

	float center = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate).x;
	float right = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(OFFSET, 0.0f)).x;
	float up = texture(sampler2D(GLOBAL_TEXTURES[height_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_height_map_texture_coordinate + vec2(0.0f, OFFSET)).x;

	vec3 tangent = normalize(vec3(1.0f, right, 0.0f) - vec3(0.0f, center, 0.0f));
	vec3 bitangent = normalize(vec3(0.0f, up, 1.0f) - vec3(0.0f, center, 0.0f));
	vec3 normal = normalize(cross(tangent, bitangent));

	normal = dot(normal, vec3(0.0f, 1.0f, 0.0f)) >= 0.0f ? normal : -normal;

	return mat3(tangent, bitangent, normal);
}

/*
*	Calculates the material.
*/
TerrainMaterial CalculateMaterial(vec2 height_map_texture_coordinate, vec2 material_texture_coordinate)
{
	//Retrieve the 4 materials to blend between.
	vec4 index_map = texture(sampler2D(GLOBAL_TEXTURES[index_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

	Material material_1 = GLOBAL_MATERIALS[int(index_map[0] * 255.0f)];
	Material material_2 = GLOBAL_MATERIALS[int(index_map[1] * 255.0f)];
	Material material_3 = GLOBAL_MATERIALS[int(index_map[2] * 255.0f)];
	Material material_4 = GLOBAL_MATERIALS[int(index_map[3] * 255.0f)];

	//Retrieve the 4 displacement values.
	float displacement_1 = texture(sampler2D(GLOBAL_TEXTURES[material_1.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_2 = texture(sampler2D(GLOBAL_TEXTURES[material_2.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_3 = texture(sampler2D(GLOBAL_TEXTURES[material_3.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;
	float displacement_4 = texture(sampler2D(GLOBAL_TEXTURES[material_4.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).w;

	//Retrieve the blend map.
	vec4 blend_map = texture(sampler2D(GLOBAL_TEXTURES[blend_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), height_map_texture_coordinate);

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

	material.albedo = 	texture(sampler2D(GLOBAL_TEXTURES[material_1.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[0]
						+ texture(sampler2D(GLOBAL_TEXTURES[material_2.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[1]
						+ texture(sampler2D(GLOBAL_TEXTURES[material_3.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[2]
						+ texture(sampler2D(GLOBAL_TEXTURES[material_4.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).rgb * blend_map[3];

	material.normal_map =	(texture(sampler2D(GLOBAL_TEXTURES[material_1.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[0]
							+ (texture(sampler2D(GLOBAL_TEXTURES[material_2.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[1]
							+ (texture(sampler2D(GLOBAL_TEXTURES[material_3.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[2]
							+ (texture(sampler2D(GLOBAL_TEXTURES[material_4.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate).xyz * 2.0f - 1.0f) * blend_map[3];

	material.normal_map = normalize(material.normal_map);

	material.material_properties = 	texture(sampler2D(GLOBAL_TEXTURES[material_1.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[0]
									+ texture(sampler2D(GLOBAL_TEXTURES[material_2.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[1]
									+ texture(sampler2D(GLOBAL_TEXTURES[material_3.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[2]
									+ texture(sampler2D(GLOBAL_TEXTURES[material_4.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), material_texture_coordinate) * blend_map[3];

	return material;
}

void main()
{
	//Calculate the tangent space matrix.
	mat3 tangent_space_matrix = CalculateTerrainTangentSpaceMatrix();

	//Retrieve the terrain normal.
	vec3 terrain_normal = tangent_space_matrix[2];

    //Calculate the material texture coordinate.
	vec2 material_texture_coordinate = fragmentWorldPosition.xz * 0.25f;

	//Calculate the material.
	TerrainMaterial material = CalculateMaterial(fragment_height_map_texture_coordinate, material_texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal = normalize(tangent_space_matrix * material.normal_map);

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragmentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentWorldPosition);

    //Write the fragments.
    sceneFeatures1 = vec4(material.albedo, 0.0f);
    sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, gl_FragCoord.z);
    sceneFeatures3 = material.material_properties;
}