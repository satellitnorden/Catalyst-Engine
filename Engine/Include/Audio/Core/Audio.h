#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

//STD.
#include <cmath>

//Namespace for all things audio.
namespace Audio
{

	//Enumeration covering all formats.
	enum class Format : uint8
	{
		INTEGER_8_BIT,
		INTEGER_16_BIT,
		INTEGER_24_BIT,
		INTEGER_32_BIT,
		FLOAT_32_BIT,
		FLOAT_64_BIT,

		UNKNOWN
	};

	//The default sample rate - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_SAMPLE_RATE{ 48'000.0f };

	//The default beats per minute - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_BEATS_PER_MINUTE{ 120.0f };

	/*
	*	Returns the bits per sample for the given format.
	*/
	FORCE_INLINE NO_DISCARD uint8 BitsPerSample(const Format format) NOEXCEPT
	{
		switch (format)
		{
			case Format::INTEGER_8_BIT:
			{
				return 8;
			}

			case Format::INTEGER_16_BIT:
			{
				return 16;
			}

			case Format::INTEGER_24_BIT:
			{
				return 24;
			}

			case Format::FLOAT_32_BIT:
			case Format::INTEGER_32_BIT:
			{
				return 32;
			}

			case Format::FLOAT_64_BIT:
			{
				return 64;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0;
			}
		}
	}

	/*
	*	Converts a sample with the given format to a float32.
	*/
	FORCE_INLINE NO_DISCARD float32 ConvertToFloat32(const Format format, const void *const RESTRICT sample) NOEXCEPT
	{
		switch (format)
		{
			case Format::INTEGER_8_BIT:
			{
				const int8 value{ *static_cast<const int8* const RESTRICT>(sample) };

				return static_cast<float32>(value) / static_cast<float32>(INT8_MAXIMUM);
			}

			case Format::INTEGER_16_BIT:
			{
				const int16 value{ *static_cast<const int16 *const RESTRICT>(sample) };

				return static_cast<float32>(value) / static_cast<float32>(INT16_MAXIMUM);
			}

			case Format::INTEGER_24_BIT:
			{
				const byte bytes[3]
				{ 
					static_cast<const byte *const RESTRICT>(sample)[0],
					static_cast<const byte *const RESTRICT>(sample)[1],
					static_cast<const byte *const RESTRICT>(sample)[2]
				};

				int32 value{ bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) };

				if (value & 0x800000)
				{
					value |= ~0xFFFFFF;
				}

				return static_cast<float32>(value) / 8388607.0f;
			}

			case Format::INTEGER_32_BIT:
			{
				const int32 value{ *static_cast<const int32 *const RESTRICT>(sample) };

				return static_cast<float32>(value) / static_cast<float32>(INT32_MAXIMUM);
			}

			case Format::FLOAT_32_BIT:
			{
				return *static_cast<const float32 *const RESTRICT>(sample);
			}

			case Format::FLOAT_64_BIT:
			{
				return static_cast<float32>(*static_cast<const float64 *const RESTRICT>(sample));
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0.0f;
			}
		}
	}

	/*
	*	Converts gain to decibels.
	*/
	FORCE_INLINE NO_DISCARD float32 GainToDecibels(const float32 gain) NOEXCEPT
	{
		return 20.0f * std::log10(gain);
	}

	/*
	*	Converts decibels to gain.
	*/
	FORCE_INLINE NO_DISCARD float32 DecibelsToGain(const float32 decibels) NOEXCEPT
	{
		return std::pow(10.0f, decibels * 0.05f);
	}

	/*
	*	Converts time (in seconds) to samples.
	*/
	FORCE_INLINE NO_DISCARD uint32 TimeToSamples(const float32 time, const float32 sample_rate) NOEXCEPT
	{
		return BaseMath::Round<uint32>(time * sample_rate);
	}

	/*
	*	Performs an equal power crossfade.
	*/
	FORCE_INLINE NO_DISCARD float32 Crossfade(const float32 A, const float32 B, const float32 alpha) NOEXCEPT
	{
		const float32 A_gain{ std::cos(alpha * 0.5f * BaseMathConstants::PI) };
		const float32 B_gain{ std::sin(alpha * 0.5f * BaseMathConstants::PI) };

		return A * A_gain + B * B_gain;
	}

}