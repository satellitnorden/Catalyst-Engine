#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

/*
*	Represents an RGBA color value, packed into 32 bits - 8 bits per color.
*/
class Color final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Color() NOEXCEPT
		:
		_Data(Pack(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f)))
	{

	}

	/*
	*	Constructor taking a Vector3<float32>. Initializes the alpha channel to 1.0f.
	*/
	FORCE_INLINE constexpr Color(const Vector3<float32> &initial_color) NOEXCEPT
		:
		_Data(Pack(Vector4<float32>(initial_color, 1.0f)))
	{
		
	}

	/*
	*	Constructor taking a Vector4<float32>.
	*/
	FORCE_INLINE constexpr Color(const Vector4<float32> &initial_color) NOEXCEPT
		:
		_Data(Pack(initial_color))
	{

	}

	/*
	*	Returns the color.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4<float32> Get() const NOEXCEPT
	{
		return Unpack(_Data);
	}

	/*
	*	Sets the color.
	*/
	FORCE_INLINE constexpr void Set(const Vector4<float32> &value) NOEXCEPT
	{
		_Data = Pack(value);
	}

	/*
	*	Applies gamma correction to this color.
	*/
	FORCE_INLINE void ApplyGammaCorrection() NOEXCEPT
	{
		const Vector4<float32> unpacked{ Unpack(_Data) };
		float32 first{ 0.0f };
		float32 second{ 0.0f };
		Vector4<float32> corrected;

		first = unpacked._X * unpacked._X;
		second = first * first;
		corrected._X = first * 0.8f + second * 0.2f;

		first = unpacked._Y * unpacked._Y;
		second = first * first;
		corrected._Y = first * 0.8f + second * 0.2f;

		first = unpacked._Z * unpacked._Z;
		second = first * first;
		corrected._Z = first * 0.8f + second * 0.2f;

		corrected._W = unpacked._W;

		_Data = Pack(corrected);
	}

private:

	//The underlying data.
	uint32 _Data;

	/*
	*	Packs a Vector4<float32> into a uint32.
	*/
	FORCE_INLINE constexpr NO_DISCARD uint32 Pack(const Vector4<float32> &color) const NOEXCEPT
	{
		uint32 output{ 0 };

		output |= static_cast<uint32>(color._R * static_cast<float32>(UINT8_MAXIMUM));
		output |= static_cast<uint32>(color._G * static_cast<float32>(UINT8_MAXIMUM)) << 8;
		output |= static_cast<uint32>(color._B * static_cast<float32>(UINT8_MAXIMUM)) << 16;
		output |= static_cast<uint32>(color._A * static_cast<float32>(UINT8_MAXIMUM)) << 24;

		return output;
	}

	/*
	*	Unpacks a uint32 into a Vector4<float32> into .
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4<float32> Unpack(const uint32 color) const NOEXCEPT
	{
		Vector4<float32> output{ 0.0f, 0.0f, 0.0f, 1.0f };

		output._R = static_cast<float32>(color & UINT8_MAXIMUM) / static_cast<float32>(UINT8_MAXIMUM);
		output._G = static_cast<float32>((color >> 8) & UINT8_MAXIMUM) / static_cast<float32>(UINT8_MAXIMUM);
		output._B = static_cast<float32>((color >> 16) & UINT8_MAXIMUM) / static_cast<float32>(UINT8_MAXIMUM);
		output._A = static_cast<float32>((color >> 24) & UINT8_MAXIMUM) / static_cast<float32>(UINT8_MAXIMUM);

		return output;
	}

};