#if !defined(CATALYST_RENDERING_UTILITIES)
#define CATALYST_RENDERING_UTILITIES

//Preprocessor defines.
#define MATERIAL_PROPERTY_NO_ALBEDO_TEXTURE_BIT (1 << 0)
#define MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT (1 << 1)
#define MATERIAL_PROPERTY_NO_MATERIAL_PROPERTIES_TEXTURE_BIT (1 << 2)

//Includes.
#include "CatalystPackingUtilities.glsl"

/*
*	Applies contrast to the given fragment.
*/
vec3 ApplyContrast(vec3 fragment, float contrast)
{
	return ((fragment - 0.5f) * contrast) + 0.5f;
}

/*
*	Calculates an (approximate) tangent space matrix from a normal.
*/
mat3 CalculateTangentSpaceMatrix(vec3 normal)
{
	float x_dot = abs(dot(normal, vec3(1.0f, 0.0f, 0.0f)));
	float y_dot = abs(dot(normal, vec3(0.0f, 1.0f, 0.0f)));
	float z_dot = abs(dot(normal, vec3(0.0f, 0.0f, 1.0f)));

	vec3 tangent = vec3(0.0f);

	tangent += vec3(0.0f, 1.0f, 0.0f) * float(x_dot > y_dot && x_dot > z_dot);
	tangent += vec3(0.0f, 0.0f, 1.0f) * float(y_dot > x_dot && y_dot > z_dot);
	tangent += vec3(1.0f, 0.0f, 0.0f) * float(z_dot > x_dot && z_dot > y_dot);

	vec3 bitangent = normalize(cross(normal, tangent));

	return mat3(tangent, bitangent, normal);
}

/*
*	Retrieves the albedo at the given coordinate from the given material.
*/
vec3 RetrieveAlbedo(Material material, vec2 coordinate)
{
	if (bool(material.properties & MATERIAL_PROPERTY_NO_ALBEDO_TEXTURE_BIT))
	{
		return UnpackColor(material.albedo_texture_index).rgb;
	}
	
	else
	{
		return texture(sampler2D(GLOBAL_TEXTURES[material.albedo_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate).rgb;
	}
}

/*
*	Retrieves the material properties at the given coordinate from the given material.
*/
vec4 RetrieveMaterialProperties(Material material, vec2 coordinate)
{
	if (bool(material.properties & MATERIAL_PROPERTY_NO_MATERIAL_PROPERTIES_TEXTURE_BIT))
	{
		return UnpackColor(material.material_properties_texture_index);
	}
	
	else
	{
		return texture(sampler2D(GLOBAL_TEXTURES[material.material_properties_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
	}
}
#endif