#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

class HammersleySequence final
{

public:

	/*
	*	Calculates the 2D coordinate given the parameters.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector2<float32> CalculateCoordinate2D(const uint32 index, const uint32 maximum) NOEXCEPT
	{
		return Vector2<float32>(static_cast<float32>(index) / static_cast<float32>(maximum), RadicalInverse(index));
	}

	/*
	*	Calculates the hemisphere coordinate given the parameters.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float32> CalculateCoordinateHemisphere(const uint32 index, const uint32 maximum) NOEXCEPT
	{
		//Calculate the 2D coordinate.
		const Vector2<float32> coordinate{ CalculateCoordinate2D(index, maximum) };

		//Map the 2D coordinate onto the hemisphere.
		const float32 phi{ coordinate._Y * 2.0f * CatalystBaseMathConstants::PI };
		const float32 cos_theta{ 1.0f - coordinate._X };
		const float32 sin_theta{ CatalystBaseMath::SquareRoot(1.0f - cos_theta * cos_theta) };

		//Return the hemisphere coordinate.
		return Vector3<float32>(CatalystBaseMath::Cosine(phi) * sin_theta, CatalystBaseMath::Sine(phi) * sin_theta, cos_theta);
	}

	/*
	*	Calculates the radical inverse of the provided bit pattern using Van der Corput's algorithm.
	*/
	FORCE_INLINE constexpr static float32 NO_DISCARD RadicalInverse(uint32 bit_pattern) NOEXCEPT
	{
		//Shuffle the bit pattern.
		bit_pattern = (bit_pattern << 16u) | (bit_pattern >> 16u);
		bit_pattern = ((bit_pattern & 0x55555555u) << 1u) | ((bit_pattern & 0xAAAAAAAAu) >> 1u);
		bit_pattern = ((bit_pattern & 0x33333333u) << 2u) | ((bit_pattern & 0xCCCCCCCCu) >> 2u);
		bit_pattern = ((bit_pattern & 0x0F0F0F0Fu) << 4u) | ((bit_pattern & 0xF0F0F0F0u) >> 4u);
		bit_pattern = ((bit_pattern & 0x00FF00FFu) << 8u) | ((bit_pattern & 0xFF00FF00u) >> 8u);

		//Return the radical inverse.
		return static_cast<float32>(bit_pattern) * 2.3283064365386963e-10f;
	}

};