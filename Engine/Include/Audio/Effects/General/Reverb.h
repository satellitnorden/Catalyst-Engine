#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/SIMD.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>
#include <Audio/DelayLine.h>
#include <Audio/DualDelayLine.h>

//Math.
#include <Math/General/PrimeNumberGenerator.h>

/*
*	Simple reverb audio effect.
*/
class Reverb final : public AudioEffect
{

public:

	//Constants.
	constexpr static float32 DEFAULT_MIX{ 0.5f };
	constexpr static float32 DEFAULT_DECAY{ 0.5f };
	constexpr static float32 DEFAULT_SHIMMER{ 0.0f };
	constexpr static float32 DEFAULT_LOW_CUT{ 0.0f };
	constexpr static float32 DEFAULT_HIGH_CUT{ 1.0f };

	//The mix.
	float32 _Mix{ DEFAULT_MIX };

	//The decay.
	float32 _Decay{ DEFAULT_DECAY };

	//The shimmer.
	float32 _Shimmer{ DEFAULT_SHIMMER };

	//The low cut.
	float32 _LowCut{ DEFAULT_LOW_CUT };

	//The high cut.
	float32 _HighCut{ DEFAULT_HIGH_CUT };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Reverb() NOEXCEPT
	{
		//Set up the temporary buffer.
		_TemporaryBuffer.Upsize<true>(1);
		_TemporaryBuffer[0].Upsize<false>(1);

		//Reset the state.
		Memory::Set(_States.Data(), 0, sizeof(float32) * 16 * 2);

		//Set up the delay lines.
		{
			//Define constants.
			constexpr uint64 PRIME_NUMBER_START_INDEX{ 10 };
			constexpr float32 SCALE{ 1.0f };

			//Generate a set of prime numbers.
			StaticArray<uint32, 32 + PRIME_NUMBER_START_INDEX> prime_numbers;

			{
				ArrayProxy<uint32> proxy{ prime_numbers };
				PrimeNumberGenerator::Generate(&proxy);
			}

			//Add every other prime number to the two channels' different delay lines.
			for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
			{
				_DelayLines[0][delay_line_index].Initialize(static_cast<float32>(prime_numbers[PRIME_NUMBER_START_INDEX + delay_line_index * 2]) / 1'000.0f * SCALE, _SampleRate);
				_DelayLines[1][delay_line_index].Initialize(static_cast<float32>(prime_numbers[PRIME_NUMBER_START_INDEX + 1 + delay_line_index * 2]) / 1'000.0f * SCALE, _SampleRate);
			}
		}

		//Set up the high cut filters.
		for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
		{
			_HighCutFilters[0][delay_line_index]._Frequency = _HighCutFilters[1][delay_line_index]._Frequency = 20'000.0f;
			_HighCutFilters[0][delay_line_index]._Quality = _HighCutFilters[1][delay_line_index]._Quality = 1.0f;
			_HighCutFilters[0][delay_line_index]._Order = _HighCutFilters[1][delay_line_index]._Order = 1;
		}

		//Set up the shimmer pitch shifters.
		for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
		{
			for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
			{
				_ShimmerPitchShifters[channel_index][delay_line_index].Initialize(60.0f / 1'000.0f, _SampleRate, 2.0f);
			}
		}
	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override
	{
		//Fill up the wet buffers.
		if (_WetBuffers.Size() != number_of_channels)
		{
			_WetBuffers.Resize<true>(number_of_channels);
		}

		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			if (_WetBuffers[channel_index].Size() != number_of_samples)
			{
				_WetBuffers[channel_index].Resize<false>(number_of_samples);
			}

			Memory::Copy(_WetBuffers[channel_index].Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Set the frequency of the high cut filters.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
			{
				_HighCutFilters[channel_index][delay_line_index]._Frequency = BaseMath::LinearlyInterpolate(1'000.0f, 20'000.0f, _HighCut);
			}
		}

		//Calculate the reverb time factor.
		float32 reverb_time_factor{ 1.0f };

		{
			//Define constants.
			constexpr float32 MINIMUM_REVERB_TIME{ 1.0f };
			constexpr float32 MAXIMUM_REVERB_TIME{ 30.0f };

			reverb_time_factor = MINIMUM_REVERB_TIME * std::pow(MAXIMUM_REVERB_TIME / MINIMUM_REVERB_TIME, _Decay);
		}

		//Calculate the per delay line reverb time factors.
		StaticArray<StaticArray<float32, 16>, 2> per_delay_line_reverb_factors;

		for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
		{
			for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
			{
				per_delay_line_reverb_factors[channel_index][delay_line_index] = BaseMath::Clamp<float32>(std::pow(0.001f, _DelayLines[channel_index][delay_line_index].GetDelayTime() / reverb_time_factor), 0.0001f, 0.9999f);
			}
		}

		//Apply the low cut filter.
		_LowCutFilter._Frequency = BaseMath::LinearlyInterpolate(20.0f, 1'000.0f, _LowCut);
		_LowCutFilter.Process(context, inputs, &_WetBuffers, number_of_channels, number_of_samples);

		//Process all samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Gather outputs from the delay lines.
			float32 delay_line_outputs[2][16];

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
				{
					delay_line_outputs[channel_index][delay_line_index] = _DelayLines[channel_index][delay_line_index].ProcessSingle(_States[channel_index][delay_line_index]);
				}
			}
			
			//Calculate the outputs.
			float32 outputs[2]{ 0.0f, 0.0f };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
				{
					outputs[channel_index] += delay_line_outputs[channel_index][delay_line_index];
				}
			}
			
			//Feed the input into the state.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (uint8 delay_line_index{ 0 }; delay_line_index < 16; ++delay_line_index)
				{
					float32 feedback{ delay_line_outputs[channel_index][delay_line_index] - (2.0f / 16.0f) * outputs[channel_index] };

					_TemporaryBuffer[0][0] = feedback;

					_HighCutFilters[channel_index][delay_line_index].Process(context, _TemporaryBuffer, &_TemporaryBuffer, 1, 1);

					const float32 input_shimmer{ _ShimmerPitchShifters[channel_index][delay_line_index].ProcessSingle(_WetBuffers[channel_index][sample_index]) };

					_States[channel_index][delay_line_index] = _WetBuffers[channel_index][sample_index] + (input_shimmer * _Shimmer) + _TemporaryBuffer[0][0] * per_delay_line_reverb_factors[channel_index][delay_line_index];
				}
			}

			//Normalize the outputs.
			for (float32 &output : outputs)
			{
				output *= 0.0625f;
			}

			//Write the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				_WetBuffers[channel_index][sample_index] = outputs[channel_index];
			}
		}

		//Mix the dry input and the wet output.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				outputs->At(channel_index).At(sample_index) = Audio::Crossfade(inputs.At(channel_index).At(sample_index), _WetBuffers.At(channel_index).At(sample_index), _Mix);
			}
		}
	}

private:

	//The wet buffers.
	DynamicArray<DynamicArray<float32>> _WetBuffers;

	//The temporary buffer.
	DynamicArray<DynamicArray<float32>> _TemporaryBuffer;

	//The low cut filter.
	HighPassFilter _LowCutFilter{ 20.0f, 1.0f, 1 };

	//The states.
	StaticArray<StaticArray<float32, 16>, 2> _States;

	//The delay lines.
	StaticArray<StaticArray<DelayLine, 16>, 2> _DelayLines;

	//The high cut filters.
	StaticArray<StaticArray<LowPassFilter, 16>, 2> _HighCutFilters;

	//The shimmer pitch shifters.
	StaticArray<StaticArray<DualDelayLine, 16>, 2> _ShimmerPitchShifters;

};