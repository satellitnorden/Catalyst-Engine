#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector4.h>

/*
*	Class representing a 2D texture with 4 channels that can be constructed on the CPU.
*/
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
	RESTRICTED const Vector4 *const RESTRICT begin() const NOEXCEPT { return _Data.begin(); }

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED Vector4 *const RESTRICT begin() NOEXCEPT { return _Data.begin(); }

	/*
	*	End iterator, const.
	*/
	RESTRICTED const Vector4 *const RESTRICT end() const NOEXCEPT { return _Data.end(); }

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED Vector4 *const RESTRICT end() NOEXCEPT { return _Data.end(); }

	/*
	*	Returns the texture data, const.
	*/
	RESTRICTED const Vector4* Data() const NOEXCEPT { return _Data.Data(); }

	/*
	*	Returns the texture data, non-const.
	*/
	RESTRICTED Vector4* Data() NOEXCEPT { return _Data.Data(); }

	/*
	*	Returns the texture value at the specified indices, const.
	*/
	const Vector4& At(const uint64 xIndex, const uint64 yIndex) const NOEXCEPT
	{
		return _Data[(yIndex * _Width) + xIndex];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	Vector4& At(const uint64 xIndex, const uint64 yIndex) NOEXCEPT
	{
		return _Data[(yIndex * _Width) + xIndex];
	}

	/*
	*	Returns the texture value at the specified coordinates, const, using linear sampling.
	*/
	const Vector4 At(const Vector2 &textureCoordinate) const NOEXCEPT
	{
		return At(textureCoordinate._X, textureCoordinate._Y);
	}

	/*
	*	Returns the texture value at the specified coordinates, const, using linear sampling.
	*/
	const Vector4 At(const float xIndex, const float yIndex) const NOEXCEPT
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

		const Vector4 &bottomLeftValue{ _Data[(yBottomLeftCoordinate * _Width) + xBottomLeftCoordinate] };
		const Vector4 &bottomRightValue{ _Data[(yBottomRightCoordinate * _Width) + xBottomRightCoordinate] };
		const Vector4 &topLeftValue{ _Data[(yTopLeftCoordinate * _Width) + xTopLeftCoordinate ] };
		const Vector4 &topRightValue{ _Data[(yTopRightCoordinate * _Width) + xTopRightCoordinate] };

		const Vector4 mixA{ Vector4::LinearlyInterpolate(bottomLeftValue, topLeftValue, yFractional) };
		const Vector4 mixB{ Vector4::LinearlyInterpolate(bottomRightValue, topRightValue, yFractional) };

		return Vector4::LinearlyInterpolate(mixA, mixB, xFractional);
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
	DynamicArray<Vector4> _Data;

	//The width of the texture.
	uint64 _Width;

	//The height of the texture.
	uint64 _Height;

};