#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>
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
		resolution(0)
	{

	}

	/*
	*	Constructor taking in the resolution of the texture.
	*/
	CPUTexture4(const uint64 initialResolution) NOEXCEPT
		:
		resolution(initialResolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		data.UpsizeFast(initialResolution * initialResolution);
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
		return data[xIndex + (yIndex * resolution)];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	Vector4& At(const uint64 xIndex, const uint64 yIndex) NOEXCEPT
	{
		return data[xIndex + (yIndex * resolution)];
	}

	/*
	*	Returns the texture value at the specified index, const, using linear sampling.
	*/
	const Vector4 At(const float xIndex, const float yIndex) const NOEXCEPT
	{
		const float texelSize{ 1.0f / static_cast<float>(resolution) };

		const float xPixelPosition{ xIndex / texelSize + 0.5f };
		const float yPixelPosition{ yIndex / texelSize + 0.5f };

		const float xFractional{ CatalystMath::Fractional(xPixelPosition) };
		const float yFractional{ CatalystMath::Fractional(yPixelPosition) };

		const float xStartTexel{ (xPixelPosition - xFractional) * texelSize };
		const float yStartTexel{ (yPixelPosition - yFractional) * texelSize };

		//Apparently this is needed, to sample the correct texel. I do not know why. Need to investigate. This keeps the camera from clipping against the terrain though.
		constexpr uint64 xModifier{ static_cast<uint64>(-1) };
		constexpr uint64 yModifier{ static_cast<uint64>(-1) };

		const uint64 xBottomLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(xStartTexel * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const uint64 yBottomLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(yStartTexel * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const uint64 xBottomRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((xStartTexel + texelSize) * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const uint64 yBottomRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(yStartTexel * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const uint64 xTopLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>(xStartTexel * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const uint64 yTopLeftCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((yStartTexel + texelSize) * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const uint64 xTopRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((xStartTexel + texelSize) * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const uint64 yTopRightCoordinate{ CatalystMath::Clamp<uint64>(static_cast<uint64>((yStartTexel + texelSize) * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const Vector4 &bottomLeftValue{ data[xBottomLeftCoordinate + (yBottomLeftCoordinate * resolution)] };
		const Vector4 &bottomRightValue{ data[xBottomRightCoordinate + (yBottomRightCoordinate * resolution)] };
		const Vector4 &topLeftValue{ data[xTopLeftCoordinate + (yTopLeftCoordinate * resolution)] };
		const Vector4 &topRightValue{ data[xTopRightCoordinate + (yTopRightCoordinate * resolution)] };

		const Vector4 mixA{ CatalystMath::LinearlyInterpolate(bottomLeftValue, topLeftValue, yFractional) };
		const Vector4 mixB{ CatalystMath::LinearlyInterpolate(bottomRightValue, topRightValue, yFractional) };

		return CatalystMath::LinearlyInterpolate(mixA, mixB, xFractional);
	}

	/*
	*	Returns the resolution of the texture.
	*/
	uint64 GetResolution() const NOEXCEPT { return resolution; }

private:

	//The underlying texture data.
	DynamicArray<Vector4> data;

	//The resolution of the texture.
	uint64 resolution;

};