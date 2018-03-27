#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector2.h>
#include <Math/Vector4.h>

/*
*	Class representing a texture with 4 channels that can be constructed on the CPU.
*/
class CPUTexture4 final
{

public:

	/*
	*	Default constructor.
	*/
	CPUTexture4() NOEXCEPT
		:
		width(0),
		height(0)
	{

	}

	/*
	*	Constructor taking in the resolution of the texture. Assumes that width and height does not differ.
	*/
	CPUTexture4(const uint64 initialResolution) NOEXCEPT
		:
		width(initialResolution),
		height(initialResolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		data.UpsizeFast(width * height);
	}

	/*
	*	Constructor taking in the resolution of the texture. Takes both the width and the height
	*/
	CPUTexture4(const uint64 initialWidth, const uint64 initialHeight) NOEXCEPT
		:
		width(initialWidth),
		height(initialHeight)
	{
		//Resize the underlying texture data to be able to hold all the data.
		data.UpsizeFast(width * height);
	}

	/*
	*	Returns the texture data, const.
	*/
	RESTRICTED const Vector4* Data() const NOEXCEPT { return data.Data(); }

	/*
	*	Returns the texture data, non-const.
	*/
	RESTRICTED Vector4* Data() NOEXCEPT { return data.Data(); }

	/*
	*	Returns the texture value at the specified indices, const.
	*/
	const Vector4& At(const uint64 xIndex, const uint64 yIndex) const NOEXCEPT
	{
		return data[(xIndex * height) + yIndex];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	Vector4& At(const uint64 xIndex, const uint64 yIndex) NOEXCEPT
	{
		return data[(xIndex * height) + yIndex];
	}

	/*
	*	Returns the texture value at the specified coordinates, const, using linear sampling.
	*/
	const Vector4 At(const Vector2 &textureCoordinate) const NOEXCEPT
	{
		return At(textureCoordinate.X, textureCoordinate.Y);
	}

	/*
	*	Returns the texture value at the specified coordinates, const, using linear sampling.
	*/
	const Vector4 At(const float xIndex, const float yIndex) const NOEXCEPT
	{
		const float xTexelSize{ 1.0f / static_cast<float>(width) };
		const float yTexelSize{ 1.0f / static_cast<float>(height) };

		const float xPixelPosition{ xIndex / xTexelSize + 0.5f };
		const float yPixelPosition{ yIndex / yTexelSize + 0.5f };

		const float xFractional{ CatalystMath::Fractional(xPixelPosition) };
		const float yFractional{ CatalystMath::Fractional(yPixelPosition) };

		const float xStartTexel{ (xPixelPosition - xFractional) * xTexelSize };
		const float yStartTexel{ (yPixelPosition - yFractional) * yTexelSize };

		//Apparently this is needed, to sample the correct texel. I do not know why. Need to investigate. This keeps the camera from clipping against the terrain though.
		constexpr uint64 xModifier{ static_cast<uint64>(-1) };
		constexpr uint64 yModifier{ static_cast<uint64>(-1) };

		const uint64 xBottomLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(xStartTexel * static_cast<float>(width)) + xModifier, 0, width - 1) };
		const uint64 yBottomLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(yStartTexel * static_cast<float>(height)) + yModifier, 0, height - 1) };

		const uint64 xBottomRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((xStartTexel + xTexelSize) * static_cast<float>(width)) + xModifier, 0, width - 1) };
		const uint64 yBottomRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(yStartTexel * static_cast<float>(height)) + yModifier, 0, height - 1) };

		const uint64 xTopLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(xStartTexel * static_cast<float>(width)) + xModifier, 0, width - 1) };
		const uint64 yTopLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((yStartTexel + yTexelSize) * static_cast<float>(height)) + yModifier, 0, height - 1) };

		const uint64 xTopRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((xStartTexel + xTexelSize) * static_cast<float>(width)) + xModifier, 0, width - 1) };
		const uint64 yTopRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((yStartTexel + yTexelSize) * static_cast<float>(height)) + yModifier, 0, height - 1) };

		const Vector4 &bottomLeftValue{ data[(xBottomLeftCoordinate * height) + yBottomLeftCoordinate] };
		const Vector4 &bottomRightValue{ data[(xBottomRightCoordinate * height) + yBottomRightCoordinate] };
		const Vector4 &topLeftValue{ data[(xTopLeftCoordinate * height) + yTopLeftCoordinate ] };
		const Vector4 &topRightValue{ data[(xTopRightCoordinate * height) + yTopRightCoordinate] };

		const Vector4 mixA{ CatalystMath::LinearlyInterpolate(bottomLeftValue, topLeftValue, yFractional) };
		const Vector4 mixB{ CatalystMath::LinearlyInterpolate(bottomRightValue, topRightValue, yFractional) };

		return CatalystMath::LinearlyInterpolate(mixA, mixB, xFractional);
	}

	/*
	*	Returns the width of the texture.
	*/
	uint64 GetWidth() const NOEXCEPT { return width; }

	/*
	*	Returns the height of the texture.
	*/
	uint64 GetHeight() const NOEXCEPT { return height; }

private:

	//The underlying texture data.
	DynamicArray<Vector4> data;

	//The width of the texture.
	uint64 width;

	//The height of the texture.
	uint64 height;

};