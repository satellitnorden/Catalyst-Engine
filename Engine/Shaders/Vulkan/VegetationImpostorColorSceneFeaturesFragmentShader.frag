//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int material_index;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec3 fragment_normal;
layout (location = 2) in vec2 fragment_texture_coordinate;

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

void main()
{
	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[material_index];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, fragment_texture_coordinate);

	//Sample the material properties.
	vec4 material_properties = RetrieveMaterialProperties(material, fragment_texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal;

	if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
	{
		shading_normal = fragment_normal;
	}

	else
	{
		//Construct the tanget space matrix.
		vec3 tangent = vec3(0.0f, 1.0f, 0.0f);
		vec3 bitangent = cross(tangent, fragment_normal);
		vec3 normal = fragment_normal;

		mat3 tangent_space_matrix = mat3(tangent, bitangent, normal);

		//Sample the normal map.
		vec3 normal_map = texture(sampler2D(GLOBAL_TEXTURES[material.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).xyz;
		shading_normal = normal_map * 2.0f - 1.0f;
		shading_normal = tangent_space_matrix * shading_normal;
		shading_normal = normalize(shading_normal);
	}

  	//Calculate the velocity.
  	vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragment_world_position) - CalculateScreenCoordinate(viewMatrixMinusOne, fragment_world_position);

    //Write the fragments.
    sceneFeatures1 = vec4(albedo, float(material_index) / 255.0f);
    sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, gl_FragCoord.z);
    sceneFeatures3 = material_properties;
}