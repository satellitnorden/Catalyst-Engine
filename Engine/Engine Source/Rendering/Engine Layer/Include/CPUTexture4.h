#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>
#include <Vector4.h>

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
	CPUTexture4(const size_t initialResolution) NOEXCEPT
		:
		resolution(initialResolution)
	{
		//Resize the underlying texture data to be able to hold all the data.
		data.Resize(initialResolution * initialResolution);
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
	const Vector4& At(const size_t xIndex, const size_t yIndex) const NOEXCEPT
	{
		return data[xIndex + (yIndex * resolution)];
	}

	/*
	*	Returns the texture value at the specified indices, non-const.
	*/
	Vector4& At(const size_t xIndex, const size_t yIndex) NOEXCEPT
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

		const float xFractional{ GameMath::Fractional(xPixelPosition) };
		const float yFractional{ GameMath::Fractional(yPixelPosition) };

		const float xStartTexel{ (xPixelPosition - xFractional) * texelSize };
		const float yStartTexel{ (yPixelPosition - yFractional) * texelSize };

		//Apparently this is needed, to sample the correct texel. I do not know why. Need to investigate. This keeps the camera from clipping against the terrain though.
		constexpr size_t xModifier{ static_cast<size_t>(-1) };
		constexpr size_t yModifier{ static_cast<size_t>(-1) };

		const size_t xBottomLeftCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>(xStartTexel * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const size_t yBottomLeftCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>(yStartTexel * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const size_t xBottomRightCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>((xStartTexel + texelSize) * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const size_t yBottomRightCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>(yStartTexel * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const size_t xTopLeftCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>(xStartTexel * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const size_t yTopLeftCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>((yStartTexel + texelSize) * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const size_t xTopRightCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>((xStartTexel + texelSize) * static_cast<float>(resolution)) + xModifier, 0, resolution - 1) };
		const size_t yTopRightCoordinate{ GameMath::Clamp<size_t>(static_cast<size_t>((yStartTexel + texelSize) * static_cast<float>(resolution)) + yModifier, 0, resolution - 1) };

		const Vector4 &bottomLeftValue{ data[xBottomLeftCoordinate + (yBottomLeftCoordinate * resolution)] };
		const Vector4 &bottomRightValue{ data[xBottomRightCoordinate + (yBottomRightCoordinate * resolution)] };
		const Vector4 &topLeftValue{ data[xTopLeftCoordinate + (yTopLeftCoordinate * resolution)] };
		const Vector4 &topRightValue{ data[xTopRightCoordinate + (yTopRightCoordinate * resolution)] };

		const Vector4 mixA{ GameMath::LinearlyInterpolate(bottomLeftValue, topLeftValue, yFractional) };
		const Vector4 mixB{ GameMath::LinearlyInterpolate(bottomRightValue, topRightValue, yFractional) };

		return GameMath::LinearlyInterpolate(mixA, mixB, xFractional);
	}

	/*
	*	Returns the resolution of the texture.
	*/
	size_t GetResolution() const NOEXCEPT { return resolution; }

private:

	//The underlying texture data.
	DynamicArray<Vector4> data;

	//The resolution of the texture.
	size_t resolution;

};