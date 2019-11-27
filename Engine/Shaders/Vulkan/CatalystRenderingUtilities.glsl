#if !defined(CATALYST_RENDERING_UTILITIES)
#define CATALYST_RENDERING_UTILITIES

//Preprocessor defines.
#define MATERIAL_PROPERTY_NO_ALBEDO_TEXTURE_BIT (1 << 0)
#define MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT (1 << 1)
#define MATERIAL_PROPERTY_NO_MATERIAL_PROPERTIES_TEXTURE_BIT (1 << 2)

//Includes.
#include "CatalystPackingUtilities.glsl"

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
		return texture(sampler2D(globalTextures[material.albedo_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate).rgb;
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
		return texture(sampler2D(globalTextures[material.material_properties_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
	}
}
#endif