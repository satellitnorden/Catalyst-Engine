//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 128) int material_index;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 3) in vec3 fragmentPreviousWorldPosition;
layout (location = 4) in vec3 fragmentCurrentWorldPosition;
layout (location = 5) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;

/*
*	Applies parallax mapping.
*/
vec2 ApplyParallaxMapping(vec2 texture_coordinate, vec3 tangent_space_view_direction)
{
	//float height =  texture(sampler2D(globalTextures[normalMapTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), texture_coordinate).w;    
	float height = 1.0f;
    vec2 p = tangent_space_view_direction.xy / tangent_space_view_direction.z * (height * 1.0f);
    return texture_coordinate - p;   
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

void main()
{
	//Calculate the final texture coordinate.
	vec2 final_texture_coordinate = fragmentTextureCoordinate;

	if (false) //Whether or not to apply parallax mapping.
	{
		//Calculate the tangent space perceiver position.
		vec3 tangent_space_perceiver_position = fragmentTangentSpaceMatrix * perceiverWorldPosition;

		//Calculate the tangent space fragment position.
		vec3 tangent_space_fragment_position = fragmentTangentSpaceMatrix * fragmentCurrentWorldPosition;

		//Calculate the tangent space view direction.
		vec3 tangent_space_view_direction = normalize(tangent_space_perceiver_position - tangent_space_fragment_position);

		//Apply parallax mapping to the texture coordinate.
		final_texture_coordinate = ApplyParallaxMapping(final_texture_coordinate, tangent_space_view_direction);
	}

   	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[material_index];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, final_texture_coordinate);

	//Sample the material properties.
	vec4 material_properties = RetrieveMaterialProperties(material, final_texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal;

	if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
	{
		shading_normal = fragmentTangentSpaceMatrix[2];
	}

	else
	{
	 //Sample the normal map.
	 vec3 normal_map = texture(sampler2D(globalTextures[material.normal_map_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), final_texture_coordinate).xyz;
	 shading_normal = normal_map * 2.0f - 1.0f;
	 shading_normal = fragmentTangentSpaceMatrix * shading_normal;
	 shading_normal = normalize(shading_normal);
	}

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragmentCurrentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentPreviousWorldPosition);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(albedo, vec3(2.2f)), 0);
    sceneFeatures2 = vec4(PackNormal(shading_normal), velocity, length(fragmentCurrentWorldPosition - perceiverWorldPosition));
    sceneFeatures3 = material_properties;
}