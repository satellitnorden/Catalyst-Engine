#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>

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
	CPUTexture2D(const uint64 initialResolution) NOEXCEPT
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
	CPUTexture2D(const uint64 initialWidth, const uint64 initialHeight) NOEXCEPT
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
	void Initialize(const uint64 initialWidth, const uint64 initialHeight) NOEXCEPT
	{
		//Set the width and height.
		_Width = initialWidth;
		_Height = initialHeight;

		//Resize the underlying texture data to be able to hold all the data.
		_Data.UpsizeFast(_Width * _Height);
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
	const Type& At(const uint64 X, const uint64 Y) const NOEXCEPT
	{
		return _Data[(Y * _Width) + X];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	Type& At(const uint64 X, const uint64 Y) NOEXCEPT
	{
		return _Data[(Y * _Width) + X];
	}

	/*
	*	Returns the texture value at the specified coordinates, const, using linear sampling.
	*/
	const Type At(const Vector2<float> &textureCoordinate) const NOEXCEPT
	{
		return At(textureCoordinate._X, textureCoordinate._Y);
	}

	/*
	*	Returns the texture value at the specified coordinates, const, using linear sampling.
	*/
	const Type At(const float xIndex, const float yIndex) const NOEXCEPT
	{
		const float xTexelSize{ 1.0f / static_cast<float>(_Width) };
		const float yTexelSize{ 1.0f / static_cast<float>(_Height) };

		const float xPixelPosition{ xIndex / xTexelSize + 0.5f };
		const float yPixelPosition{ yIndex / yTexelSize + 0.5f };

		const float xFractional{ CatalystBaseMath::Fractional(xPixelPosition) };
		const float yFractional{ CatalystBaseMath::Fractional(yPixelPosition) };

		const float xStartTexel{ (xPixelPosition - xFractional) * xTexelSize };
		const float yStartTexel{ (yPixelPosition - yFractional) * yTexelSize };

		const uint64 xBottomLeftCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>(xStartTexel * static_cast<float>(_Width)), 0, _Width - 1) };
		const uint64 yBottomLeftCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>(yStartTexel * static_cast<float>(_Height)), 0, _Height - 1) };

		const uint64 xBottomRightCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>((xStartTexel + xTexelSize) * static_cast<float>(_Width)), 0, _Width - 1) };
		const uint64 yBottomRightCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>(yStartTexel * static_cast<float>(_Height)), 0, _Height - 1) };

		const uint64 xTopLeftCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>(xStartTexel * static_cast<float>(_Width)), 0, _Width - 1) };
		const uint64 yTopLeftCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>((yStartTexel + yTexelSize) * static_cast<float>(_Height)), 0, _Height - 1) };
		const uint64 xTopRightCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>((xStartTexel + xTexelSize) * static_cast<float>(_Width)), 0, _Width - 1) };
		const uint64 yTopRightCoordinate{ CatalystBaseMath::Clamp<uint64>(static_cast<uint64>((yStartTexel + yTexelSize) * static_cast<float>(_Height)), 0, _Height - 1) };

		const Type &bottomLeftValue{ _Data[(yBottomLeftCoordinate * _Width) + xBottomLeftCoordinate] };
		const Type &bottomRightValue{ _Data[(yBottomRightCoordinate * _Width) + xBottomRightCoordinate] };
		const Type &topLeftValue{ _Data[(yTopLeftCoordinate * _Width) + xTopLeftCoordinate ] };
		const Type &topRightValue{ _Data[(yTopRightCoordinate * _Width) + xTopRightCoordinate] };

		const Type mixA{ Type::LinearlyInterpolate(bottomLeftValue, topLeftValue, yFractional) };
		const Type mixB{ Type::LinearlyInterpolate(bottomRightValue, topRightValue, yFractional) };

		return Type::LinearlyInterpolate(mixA, mixB, xFractional);
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
	uint64 _Width;

	//The height of the texture.
	uint64 _Height;

};