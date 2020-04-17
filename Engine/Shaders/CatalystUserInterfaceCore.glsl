#if !defined(CATALYST_USER_INTERFACE_CORE)
#define CATALYST_USER_INTERFACE_CORE

//Constants.
#define USER_INTERFACE_MATERIAL_TYPE_NONE (0)
#define USER_INTERFACE_MATERIAL_TYPE_COLOR (1)
#define USER_INTERFACE_MATERIAL_TYPE_TEXTURE_INDEX (2)

//Includes.
#include "CatalystPackingUtilities.glsl"

/*
*	User interface material struct definition.
*/
struct UserInterfaceMaterial
{

	//The properties.
	uint _Properties;

	//The primary color/texture index.
	uint _PrimaryColorTextureIndex;

	//The secondary color/texture index.
	uint _SecondaryColorTextureIndex;

	//The border offset.
	float _BorderOffset;

};

/*
*	Evaluates the given user interface material at the given coordinate.
*/
vec4 EvaluateUserInterfaceMaterial(UserInterfaceMaterial material, vec2 coordinate, float element_aspect_ratio)
{
	//Calculate the primary type.
	uint primary_type = uint(TEST_BIT(material._Properties, BIT(0))) + 2 * uint(TEST_BIT(material._Properties, BIT(1)));

	//Calculate the primary value.
	vec4 primary_value;

	switch (primary_type)
	{
		case USER_INTERFACE_MATERIAL_TYPE_NONE:
		{
			primary_value = vec4(0.0f);

			break;
		}

		case USER_INTERFACE_MATERIAL_TYPE_COLOR:
		{
			primary_value = UnpackColor(material._PrimaryColorTextureIndex);

			break;
		}

		case USER_INTERFACE_MATERIAL_TYPE_TEXTURE_INDEX:
		{
			primary_value = texture(sampler2D(GLOBAL_TEXTURES[material._PrimaryColorTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

			break;
		}
	}

	//Calculate the secondary type.
	uint secondary_type = uint(TEST_BIT(material._Properties, BIT(2))) + 2 * uint(TEST_BIT(material._Properties, BIT(3)));

	//Calculate the secondary value.
	vec4 secondary_value;

	switch (secondary_type)
	{
		case USER_INTERFACE_MATERIAL_TYPE_NONE:
		{
			secondary_value = vec4(0.0f);

			break;
		}

		case USER_INTERFACE_MATERIAL_TYPE_COLOR:
		{
			secondary_value = UnpackColor(material._SecondaryColorTextureIndex);

			break;
		}

		case USER_INTERFACE_MATERIAL_TYPE_TEXTURE_INDEX:
		{
			secondary_value = texture(sampler2D(GLOBAL_TEXTURES[material._SecondaryColorTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

			break;
		}
	}

	//Calculate the border weight.
	float border_weight = float(coordinate.x <= material._BorderOffset
								|| coordinate.x >= (1.0f - material._BorderOffset)
								|| coordinate.y <= (material._BorderOffset * ASPECT_RATIO * element_aspect_ratio)
								|| coordinate.y >= (1.0f - (material._BorderOffset * ASPECT_RATIO * element_aspect_ratio)));

	//Return the final value.
	return mix(primary_value, secondary_value, border_weight);
}

#endif