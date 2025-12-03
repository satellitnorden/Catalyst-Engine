#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>
#include <Audio/Effects/General/PeakFilter.h>
#include <Audio/OverSampler.h>

/*
*	A bass distortion unit that splits the signal in a low, mid and high band and processes them differently.
*/
class BassDistortion1 final : public AudioEffect
{

public:
	
	//Constants.
	constexpr static float32 LOW_BAND_LOW_PASS_MINIMUM_FREQUENCY{ 40.0f };
	constexpr static float32 LOW_BAND_LOW_PASS_MAXIMUM_FREQUENCY{ 400.0f };
	constexpr static float32 LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY{ 200.0f };

	constexpr static float32 MID_BAND_HIGH_PASS_MINIMUM_FREQUENCY{ 400.0f };
	constexpr static float32 MID_BAND_HIGH_PASS_MAXIMUM_FREQUENCY{ 1'000.0f };
	constexpr static float32 MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY{ 400.0f };

	constexpr static float32 MID_BAND_LOW_PASS_MINIMUM_FREQUENCY{ 1'000.0f };
	constexpr static float32 MID_BAND_LOW_PASS_MAXIMUM_FREQUENCY{ 4'000.0f };
	constexpr static float32 MID_BAND_LOW_PASS_DEFAULT_FREQUENCY{ 4'000.0f };

	//Denotes whether or not the low band is enabled.
	bool _LowBandEnabled{ true };

	//The low band low pass frequency.
	float32 _LowBandLowPassFrequency{ LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY };

	//The low band compression.
	float32 _LowBandCompression{ 0.5f };

	//The low band gain.
	float32 _LowBandGain{ 1.0f };

	//Denotes whether or not the mid band is enabled.
	bool _MidBandEnabled{ true };

	//The mid band high pass frequency.
	float32 _MidBandHighPassFrequency{ MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY };

	//The mid band low pass frequency.
	float32 _MidBandLowPassFrequency{ MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY };

	//The mid band distortion.
	float32 _MidBandDistortion{ 0.5f };

	//The mid band gain.
	float32 _MidBandGain{ 1.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE BassDistortion1() NOEXCEPT
	{
		
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		
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
		//If no band is enabled, just copy the inputs into the outputs and move on.
		if (!_LowBandEnabled && !_MidBandEnabled)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
			}

			return;
		}

		//Copy the inputs into the input buffer.
		_InputBuffer.Resize<true>(number_of_channels);

		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_InputBuffer.At(channel_index).Resize<false>(number_of_samples);
			Memory::Copy(_InputBuffer.At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Set up the working buffer.
		_WorkingBuffer.Resize<true>(number_of_channels);

		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_WorkingBuffer.At(channel_index).Resize<false>(number_of_samples);
		}

		//Set the outputs to zero, as we will continually add to it.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Set(outputs->At(channel_index).Data(), 0, sizeof(float32) * number_of_samples);
		}

		//Process the low band.
		if (_LowBandEnabled)
		{
			//Apply the pre low pass filter.
			_LowBand._PreLowPassFilter.Process(context, _InputBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			/*
			*	Apply compression.
			*	Since we're low-passing pretty aggressively here, we can get away with just doing a simple saturation.
			*/
			{
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(6.0f, 24.0f, _LowBandCompression)) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_WorkingBuffer.At(channel_index).At(sample_index) = std::tanh(_WorkingBuffer.At(channel_index).At(sample_index) * gain);
					}
				}
			}

			//Apply the post low pass filter.
			_LowBand._PostLowPassFilter._Frequency = _LowBandLowPassFrequency;
			_LowBand._PostLowPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the gain.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::MultiplyByScalar(_WorkingBuffer.At(channel_index).Data(), number_of_samples, _LowBandGain);
			}

			//Add to the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::Add(outputs->At(channel_index).Data(), _WorkingBuffer.At(channel_index).Data(), number_of_samples);
			}
		}

		//Process the mid band.
		if (_MidBandEnabled)
		{
			//Apply the filters.
			_MidBand._HighPassFilter1.Process(context, _InputBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._HighPassFilter2.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._HighPassFilter3.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._PeakFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply distortion.
			{
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(18.0f, 72.0f, BaseMath::InverseSquare(_MidBandDistortion))) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_MidBand._OverSamplers[channel_index].Feed(_WorkingBuffer.At(channel_index).At(sample_index));

						for (float32 &sample : _MidBand._OverSamplers[channel_index].GetSamples())
						{
							sample = std::tanh(sample * (sample >= 0.0f ? 2.0f : 0.5f) * gain);
						}

						_WorkingBuffer.At(channel_index).At(sample_index) = _MidBand._OverSamplers[channel_index].DownSample();
					}
				}
			}

			//Apply the post high pass filter.
			_MidBand._PostHighPassFilter._Frequency = _MidBandHighPassFrequency;
			_MidBand._PostHighPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the post low pass filter.
			_MidBand._PostLowPassFilter._Frequency = _MidBandLowPassFrequency;
			_MidBand._PostLowPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the gain.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::MultiplyByScalar(_WorkingBuffer.At(channel_index).Data(), number_of_samples, _MidBandGain);
			}

			//Add to the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::Add(outputs->At(channel_index).Data(), _WorkingBuffer.At(channel_index).Data(), number_of_samples);
			}
		}
	}

private:

	/*
	*	Low band class definition.
	*/
	class LowBand final
	{

	public:

		//The pre low pass filter.
		LowPassFilter _PreLowPassFilter{ LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

		//The post low pass filter.
		LowPassFilter _PostLowPassFilter{ LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

	};

	/*
	*	Mid band class definition.
	*/
	class MidBand final
	{

	public:

		//High pass filter 1.
		HighPassFilter _HighPassFilter1{ 200.0f, 1.0f, 1 };

		//High pass filter 2.
		HighPassFilter _HighPassFilter2{ 400.0f, 1.0f, 2 };

		//High pass filter 3.
		HighPassFilter _HighPassFilter3{ 400.0f, 1.0f, 3 };

		//The peak filter.
		PeakFilter _PeakFilter{ 400.0f, Audio::DecibelsToGain(-12.0f), 1.0f };

		//The oversamplers.
		StaticArray<OverSampler<4>, 2> _OverSamplers;

		//The post high pass filter.
		HighPassFilter _PostHighPassFilter{ MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

		//The post low pass filter.
		LowPassFilter _PostLowPassFilter{ MID_BAND_LOW_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

	};

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The working buffer.
	DynamicArray<DynamicArray<float32>> _WorkingBuffer;

	//The low band.
	LowBand _LowBand;

	//The mid band.
	MidBand _MidBand;

};