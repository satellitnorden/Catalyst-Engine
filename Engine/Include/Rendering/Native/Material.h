#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Material final
{

public:

	//Enumeration covering all material types.
	enum class Type : int32
	{
		Opaque
	};

	//The type.
	Type _Type{ Type::Opaque };

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
	FORCE_INLINE constexpr Material(const Type initialType, const int32 initialFirstTextureIndex, const int32 initialSecondTextureIndex, const int32 initialThirdTextureIndex) NOEXCEPT
		:
		_Type(initialType),
		_FirstTextureIndex(initialFirstTextureIndex),
		_SecondTextureIndex(initialSecondTextureIndex),
		_ThirdTextureIndex(initialThirdTextureIndex)
	{

	}

};