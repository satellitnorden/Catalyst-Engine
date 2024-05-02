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
	*	Returns the value.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetValue() const NOEXCEPT
	{
		return _Value;
	}

	/*
	*	Sets the color.
	*/
	FORCE_INLINE void SetColor(const Vector4<float32> &color) NOEXCEPT
	{
		_Value = *Color(color).Data();
		_IsTexture = false;
	}

	/*
	*	Sets the texture index.
	*/
	FORCE_INLINE void SetTextureIndex(const uint32 texture_index) NOEXCEPT
	{
		_Value = texture_index;
		_IsTexture = true;
	}

	
	/*
	*	Returns whether or not this is a texture.
	*/
	FORCE_INLINE NO_DISCARD bool IsTexture() const NOEXCEPT
	{
		return _IsTexture;
	}

private:

	//The value.
	uint32 _Value{ 0 };

	//Denotes whether or not this is a texture.
	bool _IsTexture{ false };

};