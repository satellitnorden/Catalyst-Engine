#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Intrinsics.
#include <immintrin.h>
#include <intrin.h>

/*
*	This file contains SIMD operations.
*	Dynamically checks for AVX2 support, otherwise falls back to SSE2.
*/
namespace SIMD
{
	//Enumeration covering all backends.
	enum class Backend : uint8
	{
		UNKNOWN,

		NONE,

		SSE2,
		AVX2
	};

	//The backend.
	static Backend BACKEND{ Backend::UNKNOWN };

	/*
	*	Initializes the backend.
	*/
	FORCE_INLINE void InitializeBackend() NOEXCEPT
	{
		//Return if already initialized.
		if (BACKEND != Backend::UNKNOWN)
		{
			return;
		}

		//Check check support for the AVX modes.
		bool AVX2_supported{ false };
		//bool AVX512_supported{ false };

		{
			int32 cpu_info[4];
			__cpuid(cpu_info, 0);

			if (cpu_info[0] >= 7)
			{
				__cpuidex(cpu_info, 7, 0);

				AVX2_supported = (cpu_info[1] & (1 << 5)) != 0;
				//AVX512_supported = (cpu_info[1] & (1 << 16)) != 0;
			}
		}

		//If AVX2 is supported, choose that, otherwise fall back to SSE2.
		BACKEND = AVX2_supported ? Backend::AVX2 : Backend::SSE2;
	}

	/*
	*	Computes the dot product between X and Y and returns the result.
	*/
	FORCE_INLINE NO_DISCARD float32 DotProduct(const float32 *const RESTRICT X, const float32 *const RESTRICT Y, const uint64 length) NOEXCEPT
	{
		switch (BACKEND)
		{
			case Backend::UNKNOWN:
			{
				ASSERT(false, "Need to call 'SIMD::InitializeBackend()' before!");

				return 0.0f;
			}

			case Backend::NONE:
			{
				float32 dot_product{ 0.0f };

				for (uint64 i{ 0 }; i < length; ++i)
				{
					dot_product += X[i] * Y[i];
				}

				return dot_product;
			}

			case Backend::SSE2:
			{
				__m128 accumulator{ _mm_setzero_ps() };

				uint64 i{ 0 };

				for (; (i + 4) <= length; i += 4)
				{
					const __m128 _X{ _mm_loadu_ps(&X[i]) };
					const __m128 _Y{ _mm_loadu_ps(&Y[i]) };

					accumulator = _mm_add_ps(accumulator, _mm_mul_ps(_X, _Y));
				}

				float32 temporary[4];
				_mm_storeu_ps(temporary, accumulator);

				float32 output{ temporary[0] + temporary[1] + temporary[2] + temporary[3] };

				for (; i < length; ++i)
				{
					output += X[i] * Y[i];
				}

				return output;
			}

			case Backend::AVX2:
			{
				__m256 accumulator{ _mm256_setzero_ps() };

				uint64 i{ 0 };

				for (; (i + 8) < length; i += 8)
				{
					const __m256 _X{ _mm256_loadu_ps(&X[i]) };
					const __m256 _Y{ _mm256_loadu_ps(&Y[i]) };

					accumulator = _mm256_fmadd_ps(_X, _Y, accumulator);
				}

				__m128 halves
				{
					_mm_add_ps
					(
						_mm256_castps256_ps128(accumulator),
						_mm256_extractf128_ps(accumulator, 1)
					)
				};

				halves = _mm_hadd_ps(halves, halves);
				halves = _mm_hadd_ps(halves, halves);

				float32 output{ _mm_cvtss_f32(halves) };

				for (; i < length; ++i)
				{
					output += X[i] * Y[i];
				}

				return output;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0.0f;
			}
		}
	}

	/*
	*	Multiplies a given array of floats with a scalar.
	*/
	FORCE_INLINE void MultiplyByScalar(float32 *const RESTRICT X, const uint64 length, const float32 scalar) NOEXCEPT
	{
		switch (BACKEND)
		{
			case Backend::UNKNOWN:
			{
				ASSERT(false, "Need to call 'SIMD::InitializeBackend()' before!");

				break;
			}

			case Backend::NONE:
			{
				for (uint64 i{ 0 }; i < length; ++i)
				{
					X[i] *= scalar;
				}

				break;
			}

			case Backend::SSE2:
			{
				__m128 _scalar{ _mm_set1_ps(scalar) };

				uint64 i{ 0 };

				for (; (i + 4) <= length; i += 4)
				{
					__m128 _X{ _mm_loadu_ps(&X[i]) };
					_X = _mm_mul_ps(_X, _scalar);
					_mm_storeu_ps(&X[i], _X);
				}

				for (; i < length; ++i)
				{
					X[i] *= scalar;
				}

				break;
			}

			case Backend::AVX2:
			{
				__m256 _scalar{ _mm256_set1_ps(scalar) };

				uint64 i{ 0 };

				for (; (i + 8) < length; i += 8)
				{
					__m256 _X{ _mm256_loadu_ps(&X[i]) };
					_X = _mm256_mul_ps(_X, _scalar);
					_mm256_storeu_ps(&X[i], _X);
				}

				for (; i < length; ++i)
				{
					X[i] *= scalar;
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

}