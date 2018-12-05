#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

/*
*	Class representing a 2D texture with 4 channels that can be constructed on the CPU.
*/
template <typename Type>
class CPUTexture2D final
{

public:

	/*
	*	Default constructor.
	*/
	CPUTexture2D() NOEXCEPT
		:
		_Width(0),
		_Height(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	CPUTexture2D(const CPUTexture2D &other) NOEXCEPT
		:
		_Width(other._Width),
		_Height(other._Width),
		_Data(other._Data)
	{

	}

	/*
	*	Move constructor.
	*/
	CPUTexture2D(CPUTexture2D &&other) NOEXCEPT
		:
		_Width(other._Width),
		_Height(other._Width),
		_Data(std::move(other._Data))
	{

	}

	/*
	*	Constructor taking in the resolution of the texture. Assumes that width and height does not differ.
	*/
	CPUTexture2D(const uint32 initialResolution) NOEXCEPT
		:
		_Width(initialResolution),
		_Height(initialResolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.UpsizeFast(_Width * _Height);
	}

	/*
	*	Constructor taking in the resolution of the texture. Takes both the width and the height
	*/
	CPUTexture2D(const uint32 initialWidth, const uint32 initialHeight) NOEXCEPT
		:
		_Width(initialWidth),
		_Height(initialHeight)
	{
		//Resize the underlying texture data to be able to hold all the data.
		_Data.UpsizeFast(_Width * _Height);
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const CPUTexture2D &otherTexture) NOEXCEPT
	{
		_Width = otherTexture._Width;
		_Height = otherTexture._Height;
		_Data = otherTexture._Data;
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT begin() const NOEXCEPT { return _Data.begin(); }

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT begin() NOEXCEPT { return _Data.begin(); }

	/*
	*	End iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT end() const NOEXCEPT { return _Data.end(); }

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT end() NOEXCEPT { return _Data.end(); }

	/*
	*	Initializes this CPU texture 2D.
	*/
	void Initialize(const uint32 initialWidth, const uint32 initialHeight) NOEXCEPT
	{
		//Set the width and height.
		_Width = initialWidth;
		_Height = initialHeight;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.UpsizeFast(_Width * _Height);
	}

	/*
	*	Initializes this CPU texture 2D.
	*/
	void Initialize(const uint32 initialWidth, const uint32 initialHeight, const void *const RESTRICT data) NOEXCEPT
	{
		//Set the width and height.
		_Width = initialWidth;
		_Height = initialHeight;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.UpsizeFast(_Width * _Height);

		//Copy the data.
		MemoryUtilities::CopyMemory(_Data.Data(), data, sizeof(Type) * _Width * _Height);
	}

	/*
	*	Returns the texture data, const.
	*/
	RESTRICTED const Type* Data() const NOEXCEPT { return _Data.Data(); }

	/*
	*	Returns the texture data, non-const.
	*/
	RESTRICTED Type* Data() NOEXCEPT { return _Data.Data(); }

	/*
	*	Returns the texture value at the specified indices, const.
	*/
	const Type& At(const uint32 X, const uint32 Y) const NOEXCEPT
	{
		return _Data[(Y * _Width) + X];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	Type& At(const uint32 X, const uint32 Y) NOEXCEPT
	{
		return _Data[(Y * _Width) + X];
	}

	/*
	*	Returns the texture value at the specified coordinates using linear sampling and the given address mode.
	*/
	const Type Sample(const Vector2<float> &coordinate, const AddressMode addressMode) const NOEXCEPT
	{
		//Calculate the texel step.
		const Vector2<float> textelStep{ 1.0f / static_cast<float>(_Width), 1.0f / static_cast<float>(_Height) };

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
		const Type& lowerLeftValue{ _Data[(lowerLeftIntegerCoordinate._Y * _Width) + lowerLeftIntegerCoordinate._X] };
		const Type& upperLeftValue{ _Data[(upperLeftIntegerCoordinate._Y * _Width) + upperLeftIntegerCoordinate._X] };
		const Type& upperRightValue{ _Data[(upperRightIntegerCoordinate._Y * _Width) + upperRightIntegerCoordinate._X] };
		const Type& lowerRightValue{ _Data[(lowerRightIntegerCoordinate._Y * _Width) + lowerRightIntegerCoordinate._X] };

		//Calculate the blend values.
		const float horizontalBlend{ CatalystBaseMath::Fractional(lowerLeftCoordinate._X * static_cast<float>(_Width)) };
		const float verticalBlend{ CatalystBaseMath::Fractional(lowerLeftCoordinate._Y * static_cast<float>(_Height)) };

		//Perform the blends.
		const Type blend1{ Type::LinearlyInterpolate(lowerLeftValue, lowerRightValue, horizontalBlend) };
		const Type blend2{ Type::LinearlyInterpolate(upperLeftValue, upperRightValue, horizontalBlend) };

		return Type::LinearlyInterpolate(blend1, blend2, verticalBlend);
	}

	/*
	*	Returns the texture value at the specified coordinates using linear sampling and the given address mode.
	*/
	const Type Sample(const float X, const float Y, const AddressMode addressMode) const NOEXCEPT
	{
		return Sample(Vector2<float>(X, Y), addressMode);
	}

	/*
	*	Returns the width of the texture.
	*/
	uint64 GetWidth() const NOEXCEPT { return _Width; }

	/*
	*	Returns the height of the texture.
	*/
	uint64 GetHeight() const NOEXCEPT { return _Height; }

private:

	//The underlying texture data.
	DynamicArray<Type> _Data;

	//The width of the texture.
	uint32 _Width;

	//The height of the texture.
	uint32 _Height;

	/*
	*	Applies address mode.
	*/
	static void ApplyAddressMode(const AddressMode mode, Vector2<float> *const RESTRICT coordinate) NOEXCEPT
	{
		switch (mode)
		{
			case AddressMode::ClampToEdge:
			{
				coordinate->_X = CatalystBaseMath::Clamp<float>(coordinate->_X, 0.0f, 1.0f);
				coordinate->_Y = CatalystBaseMath::Clamp<float>(coordinate->_Y, 0.0f, 1.0f);

				break;
			}

			case AddressMode::Repeat:
			{
				coordinate->_X = coordinate->_X >= 0.0f ? CatalystBaseMath::Fractional(coordinate->_X) : 1.0f + CatalystBaseMath::Fractional(coordinate->_X);
				coordinate->_Y = coordinate->_Y >= 0.0f ? CatalystBaseMath::Fractional(coordinate->_Y) : 1.0f + CatalystBaseMath::Fractional(coordinate->_Y);

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