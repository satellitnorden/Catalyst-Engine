#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

class Material final
{

public:

	//Enumeration covering all material properties.
	enum class Property : int32
	{
		None = 0,
		Highlight = BIT(0),
		Translucent = BIT(1)
	};

	//The properties.
	Property _Properties{ Property::None };

	//The index of the first texture.
	int32 _FirstTextureIndex{ INT32_MAXIMUM };

	//The index of the second texture.
	int32 _SecondTextureIndex{ INT32_MAXIMUM };

	//The index of the third texture.
	int32 _ThirdTextureIndex{ INT32_MAXIMUM };

	//The luminance multiplier.
	float _LuminanceMultiplier{ 0.0f };

	//Some padding.
	Padding<12> _Padding;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Material() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values arguments.
	*/
	FORCE_INLINE constexpr Material(const Property initialProperties, const int32 initialFirstTextureIndex, const int32 initialSecondTextureIndex, const int32 initialThirdTextureIndex, const float initialLuminanceMultiplier) NOEXCEPT
		:
		_Properties(initialProperties),
		_FirstTextureIndex(initialFirstTextureIndex),
		_SecondTextureIndex(initialSecondTextureIndex),
		_ThirdTextureIndex(initialThirdTextureIndex),
		_LuminanceMultiplier(initialLuminanceMultiplier)
	{

	}

};

ENUMERATION_BIT_OPERATIONS(Material::Property);