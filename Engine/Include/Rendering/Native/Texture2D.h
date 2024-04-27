#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Core/BaseMath.h>
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

template <typename TYPE>
class Texture2D final
{

public:

	/*
	*	Default constructor.
	*/
	Texture2D() NOEXCEPT
		:
		_Width(0),
		_Height(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	Texture2D(const Texture2D &other) NOEXCEPT
		:
		_Width(other._Width),
		_Height(other._Height),
		_Data(other._Data)
	{

	}

	/*
	*	Move constructor.
	*/
	Texture2D(Texture2D &&other) NOEXCEPT
		:
		_Width(other._Width),
		_Height(other._Height),
		_Data(std::move(other._Data))
	{

	}

	/*
	*	Constructor taking in the resolution of the texture. Assumes that width and height does not differ.
	*/
	Texture2D(const uint32 initialResolution) NOEXCEPT
		:
		_Width(initialResolution),
		_Height(initialResolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height);
	}

	/*
	*	Constructor taking in the resolution of the texture. Takes both the width and the height
	*/
	Texture2D(const uint32 initialWidth, const uint32 initialHeight) NOEXCEPT
		:
		_Width(initialWidth),
		_Height(initialHeight)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height);
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const Texture2D &other) NOEXCEPT
	{
		_Width = other._Width;
		_Height = other._Height;
		_Data = other._Data;
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(Texture2D &&other) NOEXCEPT
	{
		_Width = other._Width;
		_Height = other._Height;
		_Data = std::move(other._Data);
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const TYPE *const RESTRICT begin() const NOEXCEPT { return _Data.begin(); }

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT begin() NOEXCEPT { return _Data.begin(); }

	/*
	*	End iterator, const.
	*/
	RESTRICTED const TYPE *const RESTRICT end() const NOEXCEPT { return _Data.end(); }

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT end() NOEXCEPT { return _Data.end(); }

	/*
	*	Initializes this texture 2D.
	*/
	void Initialize(const uint32 initial_resolution) NOEXCEPT
	{
		//Set the width and height.
		_Width = initial_resolution;
		_Height = initial_resolution;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height);
	}

	/*
	*	Initializes this texture 2D.
	*/
	void Initialize(const uint32 initial_width, const uint32 initial_height) NOEXCEPT
	{
		//Set the width and height.
		_Width = initial_width;
		_Height = initial_height;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height);
	}

	/*
	*	Initializes this texture 2D.
	*/
	void Initialize(const uint32 initialWidth, const uint32 initialHeight, const void *const RESTRICT data) NOEXCEPT
	{
		//Set the width and height.
		_Width = initialWidth;
		_Height = initialHeight;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height);

		//Copy the data.
		Memory::Copy(_Data.Data(), data, sizeof(TYPE) * _Width * _Height);
	}

	/*
	*	Destroys this texture.
	*/
	FORCE_INLINE void Destroy() NOEXCEPT
	{
		//Reset the width/height.
		_Width = 0;
		_Height = 0;

		//Destroy the data.
		_Data.Destroy();
	}

	/*
	*	Returns the texture data, const.
	*/
	RESTRICTED const TYPE* Data() const NOEXCEPT { return _Data.Data(); }

	/*
	*	Returns the texture data, non-const.
	*/
	RESTRICTED TYPE* Data() NOEXCEPT { return _Data.Data(); }

	/*
	*	Returns the texture value at the specified indices, const.
	*/
	const TYPE &At(const uint32 X, const uint32 Y) const NOEXCEPT
	{
		return _Data[X + (Y * _Width)];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	TYPE &At(const uint32 X, const uint32 Y) NOEXCEPT
	{
		return _Data[X + (Y * _Width)];
	}

	/*
	*	Returns the texture value at the specified coordinates using linear sampling and the given address mode.
	*/
	const TYPE Sample(const Vector2<float32> &coordinate, const AddressMode addressMode) const NOEXCEPT
	{
		//Calculate the texel step.
		const Vector2<float32> texel_step{ 1.0f / static_cast<float32>(_Width), 1.0f / static_cast<float32>(_Height) };

		//Calculate the coordinates.
		Vector2<float32> lower_left_coordinate{ coordinate };
		Vector2<float32> upper_left_coordinate{ coordinate + Vector2<float32>(0.0f, texel_step._Y) };
		Vector2<float32> lower_right_coordinate{ coordinate + Vector2<float32>(texel_step._X, 0.0f) };
		Vector2<float32> upper_right_coordinate{ coordinate + Vector2<float32>(texel_step._X, texel_step._Y) };

		//Apply the address mode.
		ApplyAddressMode(addressMode, &lower_left_coordinate);
		ApplyAddressMode(addressMode, &upper_left_coordinate);
		ApplyAddressMode(addressMode, &lower_right_coordinate);
		ApplyAddressMode(addressMode, &upper_right_coordinate);

		//Calculate the index coordinates.
		Vector2<uint32> lower_left_integer_coordinate{ static_cast<uint32>(lower_left_coordinate._X * static_cast<float32>(_Width)), static_cast<uint32>(lower_left_coordinate._Y * static_cast<float32>(_Height)) };
		Vector2<uint32> upper_left_integer_coordinate{ static_cast<uint32>(upper_left_coordinate._X * static_cast<float32>(_Width)), static_cast<uint32>(upper_left_coordinate._Y * static_cast<float32>(_Height)) };
		Vector2<uint32> lower_right_integer_coordinate{ static_cast<uint32>(lower_right_coordinate._X * static_cast<float32>(_Width)), static_cast<uint32>(lower_right_coordinate._Y * static_cast<float32>(_Height)) };
		Vector2<uint32> upper_right_integer_coordinate{ static_cast<uint32>(upper_right_coordinate._X * static_cast<float32>(_Width)), static_cast<uint32>(upper_right_coordinate._Y * static_cast<float32>(_Height)) };

		lower_left_integer_coordinate = Vector2<uint32>::Minimum(lower_left_integer_coordinate, Vector2<uint32>(_Width - 1, _Height - 1));
		upper_left_integer_coordinate = Vector2<uint32>::Minimum(upper_left_integer_coordinate, Vector2<uint32>(_Width - 1, _Height - 1));
		lower_right_integer_coordinate = Vector2<uint32>::Minimum(lower_right_integer_coordinate, Vector2<uint32>(_Width - 1, _Height - 1));
		upper_right_integer_coordinate = Vector2<uint32>::Minimum(upper_right_integer_coordinate, Vector2<uint32>(_Width - 1, _Height - 1));

		//Sample the values.
		const TYPE &lower_left_value{ _Data[(lower_left_integer_coordinate._Y * _Width) + lower_left_integer_coordinate._X] };
		const TYPE &upper_left_value{ _Data[(upper_left_integer_coordinate._Y * _Width) + upper_left_integer_coordinate._X] };
		const TYPE &lower_right_value{ _Data[(lower_right_integer_coordinate._Y * _Width) + lower_right_integer_coordinate._X] };
		const TYPE &upper_right_value{ _Data[(upper_right_integer_coordinate._Y * _Width) + upper_right_integer_coordinate._X] };

		//Calculate the blend values.
		const float32 horizontal_blend{ BaseMath::Fractional(lower_left_coordinate._X * static_cast<float32>(_Width)) };
		const float32 vertical_blend{ BaseMath::Fractional(lower_left_coordinate._Y * static_cast<float32>(_Height)) };

		//Perform the blends.
		const TYPE blend_1{ BaseMath::LinearlyInterpolate<TYPE>(lower_left_value, lower_right_value, horizontal_blend) };
		const TYPE blend_2{ BaseMath::LinearlyInterpolate<TYPE>(upper_left_value, upper_right_value, horizontal_blend) };

		return static_cast<TYPE>(BaseMath::LinearlyInterpolate<TYPE>(blend_1, blend_2, vertical_blend));
	}

	/*
	*	Returns the texture value at the specified coordinates using linear sampling and the given address mode.
	*/
	const TYPE Sample(const float X, const float Y, const AddressMode addressMode) const NOEXCEPT
	{
		return Sample(Vector2<float>(X, Y), addressMode);
	}

	/*
	*	Returns the resolution of this texture. Assumes that _Width == _Height.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetResolution() const NOEXCEPT
	{
		ASSERT(_Width == _Height, "_Width/_Height mismatch!");

		return _Width;
	}

	/*
	*	Returns the width of this texture.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetWidth() const NOEXCEPT
	{
		return _Width;
	}

	/*
	*	Returns the height of this texture.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetHeight() const NOEXCEPT
	{
		return _Height;
	}

private:

	//The underlying texture data.
	DynamicArray<TYPE> _Data;

	//The width of this texture.
	uint32 _Width;

	//The height of this texture.
	uint32 _Height;

	/*
	*	Applies address mode.
	*/
	void ApplyAddressMode(const AddressMode mode, Vector2<float> *const RESTRICT coordinate) const NOEXCEPT
	{
		switch (mode)
		{
			case AddressMode::CLAMP_TO_EDGE:
			{
				coordinate->_X = BaseMath::Clamp<float32>(coordinate->_X, 0.0f, 1.0f - 1.0f / static_cast<float32>(_Width));
				coordinate->_Y = BaseMath::Clamp<float32>(coordinate->_Y, 0.0f, 1.0f - 1.0f / static_cast<float32>(_Height));

				break;
			}

			case AddressMode::REPEAT:
			{
				coordinate->_X = BaseMath::Clamp<float32>(BaseMath::Fractional(coordinate->_X), 0.0f, 1.0f - 1.0f / static_cast<float32>(_Width));
				coordinate->_Y = BaseMath::Clamp<float32>(BaseMath::Fractional(coordinate->_Y), 0.0f, 1.0f - 1.0f / static_cast<float32>(_Height));

				break;
			}

			default:
			{
				ASSERT(false, "Unhandled case. ):");

				break;
			}
		}
	}

};

/*
*	Specialized function for sampling Texture2D<Vector4<byte>> texures with bilinear filtering.
*/
FORCE_INLINE NO_DISCARD Vector4<float32> Sample(const Texture2D<Vector4<byte>> &texture, const Vector2<float32> &coordinate) NOEXCEPT
{
	//Cache the width/height.
	const uint32 width{ texture.GetWidth() };
	const uint32 height{ texture.GetHeight() };

	//Assume REPEAT address mode.
	Vector2<float32> actual_coordinate;

	actual_coordinate._X = BaseMath::Fractional(coordinate._X);
	actual_coordinate._Y = BaseMath::Fractional(coordinate._Y);

	//Calculate the coordinates.
	Vector2<uint32> lower_left_coordinate
	{
		BaseMath::Minimum<uint32>(static_cast<uint32>(actual_coordinate._X * static_cast<float32>(width)), width - 1),
		BaseMath::Minimum<uint32>(static_cast<uint32>(actual_coordinate._Y * static_cast<float32>(height)), height - 1)
	};
	Vector2<uint32> upper_left_coordinate
	{
		lower_left_coordinate._X,
		BaseMath::Minimum<uint32>(lower_left_coordinate._Y + 1, height - 1)
	};
	Vector2<uint32> lower_right_coordinate
	{
		BaseMath::Minimum<uint32>(lower_left_coordinate._X + 1, width - 1),
		lower_left_coordinate._Y
	};
	Vector2<uint32> upper_right_coordinate
	{
		BaseMath::Minimum<uint32>(lower_left_coordinate._X + 1, width - 1),
		BaseMath::Minimum<uint32>(lower_left_coordinate._Y + 1, height - 1)
	};

	//Sample the values.
	Vector4<float32> lower_left_value;

	{
		const Vector4<byte> &_lower_left_value{ texture.At(lower_left_coordinate._X, lower_left_coordinate._Y) };

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			lower_left_value[i] = static_cast<float32>(_lower_left_value[i]) / static_cast<float32>(BYTE_MAXIMUM);
		}
	}

	Vector4<float32> upper_left_value;

	{
		const Vector4<byte> &_upper_left_value{ texture.At(upper_left_coordinate._X, upper_left_coordinate._Y) };

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			upper_left_value[i] = static_cast<float32>(_upper_left_value[i]) / static_cast<float32>(BYTE_MAXIMUM);
		}
	}

	Vector4<float32> lower_right_value;

	{
		const Vector4<byte> &_lower_right_value{ texture.At(lower_right_coordinate._X, lower_right_coordinate._Y) };

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			lower_right_value[i] = static_cast<float32>(_lower_right_value[i]) / static_cast<float32>(BYTE_MAXIMUM);
		}
	}

	Vector4<float32> upper_right_value;

	{
		const Vector4<byte> &_upper_right_value{ texture.At(upper_right_coordinate._X, upper_right_coordinate._Y) };

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			upper_right_value[i] = static_cast<float32>(_upper_right_value[i]) / static_cast<float32>(BYTE_MAXIMUM);
		}
	}

	//Calculate the blend values.
	const float32 horizontal_blend{ BaseMath::Fractional(coordinate._X * static_cast<float32>(width)) };
	const float32 vertical_blend{ BaseMath::Fractional(coordinate._Y * static_cast<float32>(height)) };

	//Perform the blends.
	const Vector4<float32> blend_1{ BaseMath::LinearlyInterpolate<Vector4<float32>>(lower_left_value, lower_right_value, horizontal_blend) };
	const Vector4<float32> blend_2{ BaseMath::LinearlyInterpolate<Vector4<float32>>(upper_left_value, upper_right_value, horizontal_blend) };

	return static_cast<Vector4<float32>>(BaseMath::LinearlyInterpolate<Vector4<float32>>(blend_1, blend_2, vertical_blend));
}