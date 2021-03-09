#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Color.h>

class UserInterfaceMaterial final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		NONE,
		COLOR,
		TEXTURE_INDEX
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UserInterfaceMaterial() NOEXCEPT
	{

	}

	/*
	*	Returns the primary type.
	*/
	FORCE_INLINE NO_DISCARD Type GetPrimaryType() const NOEXCEPT
	{
		if (TEST_BIT(_Properties, BIT(0)))
		{
			return Type::COLOR;
		}

		if (TEST_BIT(_Properties, BIT(1)))
		{
			return Type::TEXTURE_INDEX;
		}

		else
		{
			return Type::NONE;
		}
	}

	/*
	*	Returns the secondary type.
	*/
	FORCE_INLINE NO_DISCARD Type GetSecondaryType() const NOEXCEPT
	{
		if (TEST_BIT(_Properties, BIT(2)))
		{
			return Type::COLOR;
		}

		if (TEST_BIT(_Properties, BIT(3)))
		{
			return Type::TEXTURE_INDEX;
		}

		else
		{
			return Type::NONE;
		}
	}

	/*
	*	Sets the primary color.
	*/
	FORCE_INLINE void SetPrimaryColor(const Vector4<float32> &color) NOEXCEPT
	{
		//Clear the primary color bits.
		CLEAR_BIT(_Properties, BIT(0));
		CLEAR_BIT(_Properties, BIT(1));

		//Set the bit denoting that tthis material is using primary color.
		SET_BIT(_Properties, BIT(0));

		//Set the primary color.
		_PrimaryColor = Color(color);
	}

	/*
	*	Sets the primary texture index.
	*/
	FORCE_INLINE void SetPrimaryTextureIndex(const uint32 index) NOEXCEPT
	{
		//Clear the primary color bits.
		CLEAR_BIT(_Properties, BIT(0));
		CLEAR_BIT(_Properties, BIT(1));

		//Set the bit denoting that tthis material is using primary texture index.
		SET_BIT(_Properties, BIT(1));

		//Set the primary texture index.
		_PrimaryTextureIndex = index;
	}

	/*
	*	Sets the secondary color.
	*/
	FORCE_INLINE void SetSecondaryColor(const Vector4<float32> &color) NOEXCEPT
	{
		//Clear the secondary color bits.
		CLEAR_BIT(_Properties, BIT(2));
		CLEAR_BIT(_Properties, BIT(3));

		//Set the bit denoting that tthis material is using secondary color.
		SET_BIT(_Properties, BIT(2));

		//Set the secondary color.
		_SecondaryColor = Color(color);
	}

	/*
	*	Sets the secondary texture index.
	*/
	FORCE_INLINE void SetSecondaryTextureIndex(const uint32 index) NOEXCEPT
	{
		//Clear the secondary color bits.
		CLEAR_BIT(_Properties, BIT(2));
		CLEAR_BIT(_Properties, BIT(3));

		//Set the bit denoting that tthis material is using secondary texture index.
		SET_BIT(_Properties, BIT(3));

		//Set the secondary texture index.
		_SecondaryTextureIndex = index;
	}

	/*
	*	Sets the border offset.
	*/
	FORCE_INLINE void SetBorderOffset(const float32 offset) NOEXCEPT
	{
		_BorderOffset = offset;
	}

private:

	//The properties.
	uint32 _Properties{ 0 };

	union
	{
		//The primary color.
		Color _PrimaryColor;

		//The primary texture index.
		uint32 _PrimaryTextureIndex;
	};

	union
	{
		//The secondary color.
		Color _SecondaryColor;

		//The secondary texture index.
		uint32 _SecondaryTextureIndex;
	};

	//The border offset.
	float32 _BorderOffset;

};

static_assert(sizeof(UserInterfaceMaterial) == 16, "Size of user interface material should be 16 bytes to be optimal for GPU arrays!");