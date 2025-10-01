#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

/*
*	Implements a version of derivative noise, which uses analytical derivatives of value noise to modify the noise across octaves.
*/
class DerivativeNoise final
{

public:

	/*
	*	Generates a value in the range [0.0f, 1.0f] at the given 2D coordinates.
	*	'coordinate' is the coordinate the noise should be generated on.
	*	'number_of_octaves' controls the number of octaves.
	*	'lacunarity' controls the multiplier for the coordinate at each octave.
	*	'frequency' controls the multiplier for the amplitude at each octave.
	*	'derivative_weight' controls how much the derivatives affect the noise. At 0.0f the noise is essentially equivalent to raw value noise.
	*/
	FORCE_INLINE static NO_DISCARD float32 Generate
	(
		const Vector2<float32> &coordinate,
		const uint32 number_of_octaves = 1,
		const float32 lacunarity = 2.0f,
		const float32 frequency = 0.5f,
		const float32 derivative_weight = 1.0f
	) NOEXCEPT
	{
		constexpr Matrix2x2 TRANSFORMATION{ 0.8f, -0.6f, 0.6f, 0.8f };

		Vector2<float32> _coordinate{ coordinate };
		float32 noise{ 0.0f };
		float amplitude{ 1.0f };
		Vector2<float32> derivatives{ 0.0f, 0.0f };

		for (uint8 octave_index{ 0 }; octave_index < number_of_octaves; ++octave_index)
		{
			const Vector3<float32> noise_sample{ Sample(_coordinate) };
			derivatives += Vector2<float32>(noise_sample._Y, noise_sample._Z);
			noise += amplitude * noise_sample._X / BaseMath::LinearlyInterpolate(1.0f, 1.0f + Vector2<float32>::DotProduct(derivatives, derivatives), derivative_weight);
			_coordinate = TRANSFORMATION * _coordinate * lacunarity;
			amplitude *= frequency;
		}

		return noise;
	}

private:

	/*
	*	Samples the value noise with derivatives at the given coordinate.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> Sample(const Vector2<float32> &coordinate) NOEXCEPT
	{
		const Vector2<float32> F
		{
			BaseMath::Fractional(coordinate._X),
			BaseMath::Fractional(coordinate._Y)
		};

		const Vector2<float32> U{ F * F * F * (F * (F * 6.0f - Vector2<float32>(15.0f)) + 10.0f) };
		const Vector2<float32> DU{ Vector2<float32>(30.0f) * F * F * (F * (F - 2.0f) + 1.0f) };

		const float32 A{ Sample(Vector2<int32>(BaseMath::Floor<int32>(coordinate._X) + 0, BaseMath::Floor<int32>(coordinate._Y) + 0)) };
		const float32 B{ Sample(Vector2<int32>(BaseMath::Floor<int32>(coordinate._X) + 1, BaseMath::Floor<int32>(coordinate._Y) + 0)) };
		const float32 C{ Sample(Vector2<int32>(BaseMath::Floor<int32>(coordinate._X) + 0, BaseMath::Floor<int32>(coordinate._Y) + 1)) };
		const float32 D{ Sample(Vector2<int32>(BaseMath::Floor<int32>(coordinate._X) + 1, BaseMath::Floor<int32>(coordinate._Y) + 1)) };

		return	Vector3<float32>
				(
					A + (B - A) * U._X + (C - A) * U._Y + (A - B - C + D) * U._X * U._Y,
					DU._X * ((B - A) + (A - B - C + D) * U._Y),
					DU._Y * ((C - A) + (A - B - C + D) * U._X)
				);
	}

	/*
	*	Samples the value noise at the given coordinate.
	*/
	FORCE_INLINE static NO_DISCARD float32 Sample(const Vector2<int32> &coordinate) NOEXCEPT
	{
		return BaseMath::Fractional(std::sin(static_cast<float32>(coordinate._X) * 12.9898f + static_cast<float32>(coordinate._Y) * 78.233f) * 43'758.545312f);
	}

};