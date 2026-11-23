#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

//STD.
#include <cmath>

/*
*	Tone mapper class, containing a few different tone mapping algorithms to convert from linear space to sRGB space.
*/
class ToneMapper final
{

public:

	/*
	*	Applies the AgX tone mapping algorithm.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> AgX(Vector3<float32> value) NOEXCEPT
	{
		//Apply the first layer.
		{
			//Define constants.
			constexpr Matrix3x3 TRANSFORMATION
			{
				0.842479062253094f, 0.0423282422610123f, 0.0423756549057051f,
				0.0784335999999992f,  0.878468636469772f,  0.0784336f,
				0.0792237451477643f, 0.0791661274605434f, 0.879142973793104f
			};
			constexpr float32 MIN_EV{ -12.47393f };
			constexpr float32 MAX_EV{ 4.026069f };

			//Input transform.
			value = TRANSFORMATION * value;

			//Log2 space encoding.
			for (uint8 i{ 0 }; i < 3; ++i)
			{
				value[i] = BaseMath::Clamp<float32>(std::log2(value[i]), MIN_EV, MAX_EV);
				value[i] = (value[i] - MIN_EV) / (MAX_EV - MIN_EV);
			}

			//Default constrast approximation.
			{
				const Vector3<float32> X2{ value * value }; 
				const Vector3<float32> X4{ X2 * X2 };

				value = + 15.5f * X4 * X2
						- 40.14f * X4 * value
						+ 31.96f * X4
						- 6.868f * X2 * value
						+ 0.4298f * X2
						+ 0.1191f * value
						- 0.00232f;
			}
		}

		//Apply the third layer.
		{
			//Define constants.
			constexpr Matrix3x3 TRANSFORMATION
			{
				1.19687900512017f, -0.0528968517574562f, -0.0529716355144438f,
				-0.0980208811401368f, 1.15190312990417f, -0.0980434501171241f,
				-0.0990297440797205f, -0.0989611768448433f, 1.15107367264116f
			};

			//Inverse input transform.
			value = TRANSFORMATION * value;

			//Gamma correction.
			for (uint8 i{ 0 }; i < 3; ++i)
			{
				value[i] = std::pow(value[i], 2.2f);
			}
		}

		//Return the value.
		return PostProcess(value);
	}

private:

	/*
	*	Post-processes
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> PostProcess(const Vector3<float32> &value) NOEXCEPT
	{
		return Vector3<float32>
		(
			BaseMath::Clamp(value[0], 0.0f, 1.0f),
			BaseMath::Clamp(value[1], 0.0f, 1.0f),
			BaseMath::Clamp(value[2], 0.0f, 1.0f)
		);
	}

};