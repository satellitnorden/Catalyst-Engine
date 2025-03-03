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
class Texture3D final
{

public:

	/*
	*	Default constructor.
	*/
	Texture3D() NOEXCEPT
		:
		_Width(0),
		_Height(0),
		_Depth(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	Texture3D(const Texture3D&other) NOEXCEPT
		:
		_Width(other._Width),
		_Height(other._Height),
		_Depth(other._Depth),
		_Data(other._Data)
	{

	}

	/*
	*	Move constructor.
	*/
	Texture3D(Texture3D&&other) NOEXCEPT
		:
		_Width(other._Width),
		_Height(other._Height),
		_Depth(other._Depth),
		_Data(std::move(other._Data))
	{

	}

	/*
	*	Constructor taking in the resolution of the texture. Assumes that width, height and depth does not differ.
	*/
	Texture3D(const uint32 initial_resolution) NOEXCEPT
		:
		_Width(initial_resolution),
		_Height(initial_resolution),
		_Depth(initial_resolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height * _Depth);
	}

	/*
	*	Constructor taking in the width, height and depth of the texture. Takes both the width and the height
	*/
	Texture3D(const uint32 initial_width, const uint32 initial_height, const uint32 initial_depth) NOEXCEPT
		:
		_Width(initial_width),
		_Height(initial_height),
		_Depth(initial_depth)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.Upsize<false>(_Width * _Height * _Depth);
	}

	/*
	*	Constructor taking in the width, height, depth and data of the texture. Takes both the width and the height
	*/
	Texture3D(const uint32 initial_width, const uint32 initial_height, const uint32 initial_depth, const void* const RESTRICT data) NOEXCEPT
		:
		_Width(initial_width),
		_Height(initial_height),
		_Depth(initial_depth)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.template Upsize<false>(_Width * _Height * _Depth);

		//Copy the data.
		Memory::Copy(_Data.Data(), data, sizeof(TYPE) * _Width * _Height * _Depth);
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const Texture3D &other) NOEXCEPT
	{
		_Width = other._Width;
		_Height = other._Height;
		_Depth = other._Depth;
		_Data = other._Data;
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
	*	Initializes this texture 3D.
	*/
	void Initialize(const uint32 initial_width, const uint32 initial_height, const uint32 initial_depth) NOEXCEPT
	{
		//Set the width, height and depth.
		_Width = initial_width;
		_Height = initial_height;
		_Depth = initial_depth;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.Upsize<false>(_Width * _Height * _Depth);
	}

	/*
	*	Initializes this texture 3D.
	*/
	void Initialize(const uint32 initial_width, const uint32 initial_height, const uint32 initial_depth, const void *const RESTRICT data) NOEXCEPT
	{
		//Set the width, height and depth.
		_Width = initial_width;
		_Height = initial_height;
		_Depth = initial_depth;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.Upsize<false>(_Width * _Height * _Depth);

		//Copy the data.
		Memory::Copy(_Data.Data(), data, sizeof(TYPE) * _Width * _Height * _Depth);
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
	const TYPE& At(const uint32 X, const uint32 Y, const uint32 Z) const NOEXCEPT
	{
		return _Data[X + (Y * _Width) + (Z * _Width * _Height)];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	TYPE& At(const uint32 X, const uint32 Y, const uint32 Z) NOEXCEPT
	{
		return _Data[X + (Y * _Width) + (Z * _Width * _Height)];
	}

	/*
	*	Returns the texture value at the specified coordinates using linear sampling and the given address mode.
	*/
	const TYPE Sample(const Vector3<float> &coordinate, const AddressMode address_mode) const NOEXCEPT
	{
		/*
		//Calculate the texel step.
		const Vector3<float> textelStep{ 1.0f / static_cast<float>(_Width), 1.0f / static_cast<float>(_Height), 1.0f / static_cast<float>(_Depth) };

		//Calculate the coordinates.
		Vector2<float> lowerLeftCoordinate{ coordinate };
		Vector2<float> upperLeftCoordinate{ coordinate + Vector2<float>(0.0f, textelStep._Y) };
		Vector2<float> upperRightCoordinate{ coordinate + Vector2<float>(textelStep._X, textelStep._Y) };
		Vector2<float> lowerRightCoordinate{ coordinate + Vector2<float>(textelStep._X, 0.0f) };

		//Apply the address mode.
		ApplyAddressMode(addressMode, &lowerLeftCoordinate);
		ApplyAddressMode(addressMode, &upperLeftCoordinate);
		ApplyAddressMode(addressMode, &upperRightCoordinate);
		ApplyAddressMode(addressMode, &lowerRightCoordinate);

		//Calculate the index coordinates.
		const Vector2<uint32> lowerLeftIntegerCoordinate{ static_cast<uint32>(lowerLeftCoordinate._X * static_cast<float>(_Width)), static_cast<uint32>(lowerLeftCoordinate._Y * static_cast<float>(_Height)) };
		const Vector2<uint32> upperLeftIntegerCoordinate{ static_cast<uint32>(upperLeftCoordinate._X * static_cast<float>(_Width)), static_cast<uint32>(upperLeftCoordinate._Y * static_cast<float>(_Height)) };
		const Vector2<uint32> upperRightIntegerCoordinate{ static_cast<uint32>(upperRightCoordinate._X * static_cast<float>(_Width)), static_cast<uint32>(upperRightCoordinate._Y * static_cast<float>(_Height)) };
		const Vector2<uint32> lowerRightIntegerCoordinate{ static_cast<uint32>(lowerRightCoordinate._X * static_cast<float>(_Width)), static_cast<uint32>(lowerRightCoordinate._Y * static_cast<float>(_Height)) };

		//Sample the values.
		const TYPE& lowerLeftValue{ _Data[(lowerLeftIntegerCoordinate._Y * _Width) + lowerLeftIntegerCoordinate._X] };
		const TYPE& upperLeftValue{ _Data[(upperLeftIntegerCoordinate._Y * _Width) + upperLeftIntegerCoordinate._X] };
		const TYPE& upperRightValue{ _Data[(upperRightIntegerCoordinate._Y * _Width) + upperRightIntegerCoordinate._X] };
		const TYPE& lowerRightValue{ _Data[(lowerRightIntegerCoordinate._Y * _Width) + lowerRightIntegerCoordinate._X] };

		//Calculate the blend values.
		const float horizontalBlend{ BaseMath::Fractional(lowerLeftCoordinate._X * static_cast<float>(_Width)) };
		const float verticalBlend{ BaseMath::Fractional(lowerLeftCoordinate._Y * static_cast<float>(_Height)) };

		//Perform the blends.
		const TYPE blend1{ TYPE::LinearlyInterpolate(lowerLeftValue, lowerRightValue, horizontalBlend) };
		const TYPE blend2{ TYPE::LinearlyInterpolate(upperLeftValue, upperRightValue, horizontalBlend) };

		return TYPE::LinearlyInterpolate(blend1, blend2, verticalBlend);
		*/

		return TYPE();
	}

	/*
	*	Returns the width of the texture.
	*/
	uint32 GetWidth() const NOEXCEPT { return _Width; }

	/*
	*	Returns the height of the texture.
	*/
	uint32 GetHeight() const NOEXCEPT { return _Height; }

	/*
	*	Returns the depth of the texture.
	*/
	uint32 GetDepth() const NOEXCEPT { return _Depth; }

private:

	//The underlying texture data.
	DynamicArray<TYPE> _Data;

	//The width of this texture.
	uint32 _Width;

	//The height of this texture.
	uint32 _Height;

	//The depth of this texture.
	uint32 _Depth;

	/*
	*	Applies address mode.
	*/
	void ApplyAddressMode(const AddressMode mode, Vector3<float> *const RESTRICT coordinate) const NOEXCEPT
	{
		switch (mode)
		{
			case AddressMode::CLAMP_TO_EDGE:
			{
				coordinate->_X = BaseMath::Clamp<float>(coordinate->_X, 0.0f, 1.0f - 1.0f / static_cast<float>(_Width));
				coordinate->_Y = BaseMath::Clamp<float>(coordinate->_Y, 0.0f, 1.0f - 1.0f / static_cast<float>(_Height));
				coordinate->_Z = BaseMath::Clamp<float>(coordinate->_Z, 0.0f, 1.0f - 1.0f / static_cast<float>(_Depth));

				break;
			}

			case AddressMode::REPEAT:
			{
				coordinate->_X = coordinate->_X >= 0.0f ? BaseMath::Fractional(coordinate->_X) : BaseMath::Fractional(1.0f + BaseMath::Fractional(coordinate->_X));
				coordinate->_Y = coordinate->_Y >= 0.0f ? BaseMath::Fractional(coordinate->_Y) : BaseMath::Fractional(1.0f + BaseMath::Fractional(coordinate->_Y));
				coordinate->_Z = coordinate->_Z >= 0.0f ? BaseMath::Fractional(coordinate->_Z) : BaseMath::Fractional(1.0f + BaseMath::Fractional(coordinate->_Z));

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