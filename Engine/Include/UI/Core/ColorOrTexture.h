#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace UI
{

	/*
	*	Simple holder for either a color or a texture.
	*/
	class ColorOrTexture final
	{

	public:

		/*
		*	Default constructor.
		*/
		FORCE_INLINE ColorOrTexture() NOEXCEPT
			:
			_Type(Type::COLOR),
			_Color(1.0f, 1.0f, 1.0f, 1.0f)
		{

		}

		/*
		*	Constructor taking a color.
		*/
		FORCE_INLINE ColorOrTexture(const Vector4<float32> &color) NOEXCEPT
			:
			_Type(Type::COLOR),
			_Color(color)
		{

		}

		/*
		*	Constructor taking a texture index.
		*/
		FORCE_INLINE ColorOrTexture(const uint32 texture_index) NOEXCEPT
			:
			_Type(Type::TEXTURE),
			_TextureIndex(texture_index)
		{

		}

		/*
		*	Returns whether or not this is a color.
		*/
		FORCE_INLINE NO_DISCARD bool IsColor() const NOEXCEPT
		{
			return _Type == Type::COLOR;
		}

		/*
		*	Returns whether or not this is a texture.
		*/
		FORCE_INLINE NO_DISCARD bool IsTexture() const NOEXCEPT
		{
			return _Type == Type::TEXTURE;
		}

		/*
		*	Returns the color.
		*/
		FORCE_INLINE NO_DISCARD const Vector4<float32> &GetColor() const NOEXCEPT
		{
			ASSERT(IsColor(), "Can't get color if this is not a color!");
			return _Color;
		}

		/*
		*	Returns the texture index.
		*/
		FORCE_INLINE NO_DISCARD uint32 GetTextureIndex() const NOEXCEPT
		{
			ASSERT(IsTexture(), "Can't get texture index if this is not a texture!");
			return _TextureIndex;
		}

	private:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			COLOR,
			TEXTURE
		};

		//The type.
		Type _Type;

		//The color.
		Vector4<float32> _Color;

		//The texture index.
		uint32 _TextureIndex;

	};
}