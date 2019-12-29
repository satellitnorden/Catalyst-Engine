#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Vector.h>

class Material final
{

public:

	//Enumeration covering all types.
	enum class Type : int32
	{
		/*
		*	Visual entities with a masked material will use the optional texture index as the mask texture.
		*	Will usually run in a depth pass and a color pass.
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
		/*
		*	Defines that this material does not have any properties.
		*/
		NONE = 0,

		/*
		*	Defines that this material has no albedo texture, and instead the albedo is packed into the albedo texture index.
		*/
		NO_ALBEDO_TEXTURE = BIT(0),

		/*
		*	Defines that this material has no normal map texture, and instead visual entities will just use the normal of the model directly.
		*/
		NO_NORMAL_MAP_TEXTURE = BIT(1),

		/*
		*	Defines that this material has no material properties texture, and instead the material properties is packed into the material properties texture index.
		*/
		NO_MATERIAL_PROPERTIES_TEXTURE = BIT(2),

		/*
		*	Defines that this material is double-sided, meaning no backface culling will occur.
		*/
		DOUBLE_SIDED = BIT(3)
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
	Property _Properties{ Property::NONE };

	//The luminance multiplier.
	float _LuminanceMultiplier{ 0.0f };

	//The thickness.
	float _Thickness{ 1.0f };

	/*
	*	Packs a color into an integer.
	*/
	FORCE_INLINE constexpr static NO_DISCARD int32 PackColor(const Vector4<float> &color, const bool apply_gamma_correction) NOEXCEPT
	{
		if (apply_gamma_correction)
		{
			float first{ 0.0f };
			float second{ 0.0f };
			Vector4<float> corrected;

			first = color._X * color._X;
			second = first * first;
			corrected._X = first * 0.8f + second * 0.2f;

			first = color._Y * color._Y;
			second = first * first;
			corrected._Y = first * 0.8f + second * 0.2f;

			first = color._Z * color._Z;
			second = first * first;
			corrected._Z = first * 0.8f + second * 0.2f;

			first = color._W * color._W;
			second = first * first;
			corrected._W = first * 0.8f + second * 0.2f;

			return	static_cast<int32>(corrected._X * 255.0f)
					| (static_cast<int32>(corrected._Y * 255.0f) << 8)
					| (static_cast<int32>(corrected._Z * 255.0f) << 16)
					| (static_cast<int32>(corrected._W * 255.0f) << 24);
		}

		else
		{
			return	static_cast<int32>(color._X * 255.0f)
					| (static_cast<int32>(color._Y * 255.0f) << 8)
					| (static_cast<int32>(color._Z * 255.0f) << 16)
					| (static_cast<int32>(color._W * 255.0f) << 24);
		}
	}

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
									const float initial_luminance_multiplier,
									const float initial_thickness) NOEXCEPT
		:
		_AlbedoTextureIndex(initial_albedo_texture_index),
		_NormalMapTextureIndex(initial_normal_map_texture_index),
		_MaterialPropertiesTextureIndex(initial_material_properties_texture_index),
		_OptionalTextureIndex(initial_optional_texture_index),
		_Type(initial_type),
		_Properties(initial_properties),
		_LuminanceMultiplier(initial_luminance_multiplier),
		_Thickness(initial_thickness)
	{

	}

};

ENUMERATION_BIT_OPERATIONS(Material::Property);

static_assert(sizeof(Material) == 32, "The size of the material struct must be 32-byte-aligned!");