#if !defined(CATALYST_MATERIAL_CORE)
#define CATALYST_MATERIAL_CORE

//Includes.
#include "CatalystPackingUtilities.glsl"

/*
*	Evaluates the given material at the given coordinate.
*/
void EvaluateMaterial(	Material material,
						vec2 coordinate,
						out vec4 albedo_thickness,
						out vec4 normal_map_displacement,
						out vec4 material_properties,
						out vec4 opacity)
{
	//Does this material have an albedo/thickness texture?
	if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE))
	{
		albedo_thickness = texture(sampler2D(GLOBAL_TEXTURES[material._AlbedoThickness], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
	}

	else
	{
		albedo_thickness = UnpackColor(material._AlbedoThickness);
	}

	//Does this material have a normal map/displacement texture?
	if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))
	{
		normal_map_displacement = texture(sampler2D(GLOBAL_TEXTURES[material._NormalMapDisplacement], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
	}

	else
	{
		normal_map_displacement = UnpackColor(material._NormalMapDisplacement);
	}

	//Does this material have a material properties texture?
	if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE))
	{
		material_properties = texture(sampler2D(GLOBAL_TEXTURES[material._MaterialProperties], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
	}

	else
	{
		material_properties = UnpackColor(material._MaterialProperties);
	}

	//Does this material have an opacity texture?
	if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_OPACITY_TEXTURE))
	{
		opacity = texture(sampler2D(GLOBAL_TEXTURES[material._Opacity], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), coordinate);
	}

	else
	{
		opacity = UnpackColor(material._Opacity);
	}
}

#endif