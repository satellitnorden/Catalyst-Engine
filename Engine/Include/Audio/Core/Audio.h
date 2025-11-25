#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/SIMD.h>

//Math.
#include <Math/Core/BaseMath.h>

//STD.
#include <cmath>

//Namespace for all things audio.
namespace Audio
{

	//Enumeration covering all backends.
	enum class Backend : uint8
	{
		NONE,

		WASAPI,
		ASIO
	};

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

	//Enumeration covering all note durations.
	enum class NoteDuration : uint8
	{
		WHOLE,
		HALF,
		QUARTER,
		EIGHTH,
		SIXTEENTH,
		THIRTYSECOND,
		SIXTYFOURTH,
		HUNDREDTWENTYEIGHTH
	};

	//Enumeration covering all note modifiers.
	enum class NoteModifier : uint8
	{
		NONE,
		DOTTED,
		TRIPLET
	};

	//The default sample rate - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_SAMPLE_RATE{ 48'000.0f };

	//The default beats per minute - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_BEATS_PER_MINUTE{ 120.0f };

	//Type aliases.
	using Identifier = uint64;

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
	*	Converts multiple samples with the given format to a float32.
	*/
	FORCE_INLINE void ConvertToFloat32(const Format format, void *const RESTRICT inputs, float32 *const RESTRICT outputs, const uint32 number_of_samples) NOEXCEPT
	{
		//Cache the bytes per sample.
		const uint8 bytes_per_sample{ static_cast<uint8>(Audio::BitsPerSample(format) >> 3) };

		//Convert depending on the format.
		switch (format)
		{
			default:
			{
				for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
				{
					outputs[sample_index] = ConvertToFloat32(format, AdvancePointer(inputs, bytes_per_sample * sample_index));
				}

				break;
			}

			case Format::INTEGER_32_BIT:
			{
				//First convert the to float32's.
				SIMD::ConvertInt32ToFloat32(static_cast<const int32 *const RESTRICT>(inputs), outputs, number_of_samples);

				//Next normalize to the [-1.0f, 1.0f] range.
				SIMD::MultiplyByScalar(outputs, number_of_samples, 1.0f / static_cast<float32>(INT32_MAXIMUM));

				break;
			}

			case Format::FLOAT_32_BIT:
			{
				Memory::Copy(outputs, inputs, sizeof(float32) * number_of_samples);

				break;
			}

			//TODO: Implement SIMD-ified versions of the other formats.
		}
	}

	/*
	*	Converts a float32 to a sample with the given format.
	*/
	FORCE_INLINE void ConvertToSample(const Format format, const float32 input_sample, void *const RESTRICT output_sample) NOEXCEPT
	{
		switch (format)
		{
			case Format::INTEGER_8_BIT:
			{
				(*static_cast<int8 *const RESTRICT>(output_sample)) = static_cast<int8>(input_sample * static_cast<float32>(INT16_MAXIMUM));

				break;
			}

			case Format::INTEGER_16_BIT:
			{
				(*static_cast<int16 *const RESTRICT>(output_sample)) = static_cast<int32>(input_sample * static_cast<float32>(INT16_MAXIMUM));

				break;
			}

			case Format::INTEGER_24_BIT:
			{
				const int32 source{ static_cast<int32>(input_sample * static_cast<float32>(INT24_MAXIMUM)) };

				uint8 *const RESTRICT destination{ static_cast<uint8 *const RESTRICT>(output_sample) };

				destination[0] = static_cast<uint8>(source & 0xFF);
				destination[1] = static_cast<uint8>((source >> 8) & 0xFF);
				destination[2] = static_cast<uint8>((source >> 16) & 0xFF);

				break;
			}

			case Format::INTEGER_32_BIT:
			{
				(*static_cast<int32 *const RESTRICT>(output_sample)) = static_cast<int32>(input_sample * static_cast<float32>(INT32_MAXIMUM));

				break;
			}

			case Format::FLOAT_32_BIT:
			{
				(*static_cast<float32 *const RESTRICT>(output_sample)) = input_sample;

				break;
			}

			case Format::FLOAT_64_BIT:
			{
				(*static_cast<float64 *const RESTRICT>(output_sample)) = static_cast<float64>(input_sample);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Converts multiple float32's to samples with the given format.
	*	Note: 'inputs' here are non-const, as the conversion may need some working memory, so don't rely on it being the same after this conversion.
	*/
	FORCE_INLINE void ConvertToSample(const Format format, float32 *const RESTRICT inputs, void *const RESTRICT outputs, const uint32 number_of_samples) NOEXCEPT
	{
		//Cache the bytes per sample.
		const uint8 bytes_per_sample{ static_cast<uint8>(Audio::BitsPerSample(format) >> 3) };

		//Convert depending on the format.
		switch (format)
		{
			default:
			{
				for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
				{
					ConvertToSample(format, inputs[sample_index], AdvancePointer(outputs, bytes_per_sample * sample_index));
				}

				break;
			}

			case Format::INTEGER_32_BIT:
			{
				//First bring it up into the range of [-INT32_MAXIMUM, INT32_MAXIMUM].
				SIMD::MultiplyByScalar(inputs, number_of_samples, static_cast<float32>(INT32_MAXIMUM));

				//Next convert the to int32's.
				SIMD::ConvertFloat32ToInt32(inputs, static_cast<int32 *const RESTRICT>(outputs), number_of_samples);

				break;
			}

			case Format::FLOAT_32_BIT:
			{
				Memory::Copy(outputs, inputs, sizeof(float32) * number_of_samples);

				break;
			}

			//TODO: Implement SIMD-ified versions of the other formats.
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
	*	Calculates the note time in seconds in regards to the beats per minute.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE CalculateNoteTime(const TYPE beats_per_minute, const NoteDuration duration, const NoteModifier modifier = NoteModifier::NONE) NOEXCEPT
	{
		//Define constants.
		constexpr TYPE DOTTED_MULTIPLIER{ static_cast<TYPE>(1.5) };
		constexpr TYPE TRIPLET_MULTIPLIER{ static_cast<TYPE>(2.0 / 3.0) };

		//Calculate the time.
		TYPE calculated_time;

		//Apply the duration.
		switch (duration)
		{
			case NoteDuration::WHOLE:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(4);

				break;
			}

			case NoteDuration::HALF:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(2);

				break;
			}

			case NoteDuration::QUARTER:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute);

				break;
			}

			case NoteDuration::EIGHTH:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.5);

				break;
			}

			case NoteDuration::SIXTEENTH:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.25);

				break;
			}

			case NoteDuration::THIRTYSECOND:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.125);

				break;
			}

			case NoteDuration::SIXTYFOURTH:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.0625);

				break;
			}

			case NoteDuration::HUNDREDTWENTYEIGHTH:
			{
				calculated_time = (static_cast<TYPE>(60) / beats_per_minute) * static_cast<TYPE>(0.03125);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				calculated_time = (static_cast<TYPE>(60) / beats_per_minute);

				break;
			}
		}

		//Apply the modifier.
		switch (modifier)
		{
			case NoteModifier::NONE:
			{
				//Nothing to do here.

				break;
			}

			case NoteModifier::DOTTED:
			{
				calculated_time *= DOTTED_MULTIPLIER;

				break;
			}

			case NoteModifier::TRIPLET:
			{
				calculated_time *= TRIPLET_MULTIPLIER;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Return the calculated time.
		return calculated_time;
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