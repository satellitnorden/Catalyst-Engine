#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

class Material final
{

public:

	//Enumeration covering all types.
	enum class Type : int32
	{
		/*
		*	Visual entities with a masked material will use the optional texture index as the mask texture.
		*	Will usually run in a pre-depth pass and a color pass.
		*/
		Masked,

		/*
		*	Visual entities with an opaque material will just use the albedo, normal map and material properties textures.
		*/
		Opaque
	};

	//Enumeration covering all material properties.
	enum class Property : int32
	{
		None = 0,
		Highlight = BIT(0)
	};

	//The index of the albedo texture.
	int32 _AlbedoTextureIndex{ INT32_MAXIMUM };

	//The index of the normal map texture.
	int32 _NormalMapTextureIndex{ INT32_MAXIMUM };

	//The index of the material properties texture.
	int32 _MaterialPropertiesTextureIndex{ INT32_MAXIMUM };

	//The index of the optional texture.
	int32 _OptionalTextureIndex{ INT32_MAXIMUM };

	//The type.
	Type _Type{ Type::Opaque };

	//The properties.
	Property _Properties{ Property::None };

	//The luminance multiplier.
	float _LuminanceMultiplier{ 0.0f };

	//Some padding.
	Padding<4> _Padding;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Material() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values arguments.
	*/
	FORCE_INLINE constexpr Material(const int32 initial_albedo_texture_index,
									const int32 initial_normal_map_texture_index,
									const int32 initial_material_properties_texture_index,
									const int32 initial_optional_texture_index,
									const Type initial_type,
									const Property initial_properties,
									const float initial_luminance_multiplier) NOEXCEPT
		:
		_AlbedoTextureIndex(initial_albedo_texture_index),
		_NormalMapTextureIndex(initial_normal_map_texture_index),
		_MaterialPropertiesTextureIndex(initial_material_properties_texture_index),
		_OptionalTextureIndex(initial_optional_texture_index),
		_Type(initial_type),
		_Properties(initial_properties),
		_LuminanceMultiplier(initial_luminance_multiplier)
	{

	}

};

ENUMERATION_BIT_OPERATIONS(Material::Property);

static_assert(sizeof(Material) == 32, "The size of the material struct must be 16-byte-aligned!");