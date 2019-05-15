#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Material final
{

public:

	//Enumeration covering all material properties.
	enum class Property : int32
	{
		None = 0,
		Highlight = BIT(0)
	};

	//The properties.
	Property _Properties{ Property::None };

	//The index of the first texture.
	int32 _FirstTextureIndex{ INT32_MAXIMUM };

	//The index of the second texture.
	int32 _SecondTextureIndex{ INT32_MAXIMUM };

	//The index of the third texture.
	int32 _ThirdTextureIndex{ INT32_MAXIMUM };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Material() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values arguments.
	*/
	FORCE_INLINE constexpr Material(const Property initialProperties, const int32 initialFirstTextureIndex, const int32 initialSecondTextureIndex, const int32 initialThirdTextureIndex) NOEXCEPT
		:
		_Properties(initialProperties),
		_FirstTextureIndex(initialFirstTextureIndex),
		_SecondTextureIndex(initialSecondTextureIndex),
		_ThirdTextureIndex(initialThirdTextureIndex)
	{

	}

};