#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/HighShelfFilter.h>

/*
*	Gain matcher that tries to match the (perceived) loudness of the output signal to the input signal.
*	Useful for, for example, compressors that wants to implement an auto gain feature.
*/
class GainMatcher final : public AudioEffect
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE GainMatcher() NOEXCEPT
	{
		//Define constants.
		constexpr float32 LOUDNESS_TIME{ 100.0f / 1'000.0f };
		constexpr float32 COMPENSATION_TIME{ 1'000.0f / 1'000.0f };

		//Calculate the loudness alpha.
		_LoudnessAlpha = std::exp(-1.0f / (LOUDNESS_TIME * _SampleRate));

		//Calculate the compensation alpha.
		_CompensationAlpha = std::exp(-1.0f / (COMPENSATION_TIME * _SampleRate));
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
		//Fill up the input buffer.
		_InputBuffer.Resize<true>(inputs.Size());

		for (uint64 channel_index{ 0 }; channel_index < inputs.Size(); ++channel_index)
		{
			_InputBuffer.At(channel_index).Resize<false>(inputs.At(channel_index).Size());
			Memory::Copy(_InputBuffer.At(channel_index).Data(), inputs.At(channel_index).Data(), inputs.At(channel_index).Size() * sizeof(float32));
		}

		//Fill up the output buffer.
		_OutputBuffer.Resize<true>(outputs->Size());

		for (uint64 channel_index{ 0 }; channel_index < outputs->Size(); ++channel_index)
		{
			_OutputBuffer.At(channel_index).Resize<false>(outputs->At(channel_index).Size());
			Memory::Copy(_OutputBuffer.At(channel_index).Data(), outputs->At(channel_index).Data(), outputs->At(channel_index).Size() * sizeof(float32));
		}

		//Apply the perceptual loudness weighting on both the input and the output.
		_InputPerceptualLoudnessWeighting._HighPassFilter.Process(context, _InputBuffer, &_InputBuffer, number_of_channels, number_of_samples);
		_InputPerceptualLoudnessWeighting._HighShelfFilter.Process(context, _InputBuffer, &_InputBuffer, number_of_channels, number_of_samples);

		_OutputPerceptualLoudnessWeighting._HighPassFilter.Process(context, _OutputBuffer, &_OutputBuffer, number_of_channels, number_of_samples);
		_OutputPerceptualLoudnessWeighting._HighShelfFilter.Process(context, _OutputBuffer, &_OutputBuffer, number_of_channels, number_of_samples);

		//Process all samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Calculate the input and output energy.
			float32 input_energy{ 0.0f };
			float32 output_energy{ 0.0f };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				const float32 input{ _InputBuffer[channel_index][sample_index] };
				const float32 output{ _OutputBuffer[channel_index][sample_index] };

				input_energy += input * input;
				output_energy += output * output;
			}

			input_energy /= static_cast<float32>(number_of_channels);
			output_energy /= static_cast<float32>(number_of_channels);

			//Update the input and output envelope.
			_InputEnvelope = _InputEnvelope * _LoudnessAlpha + input_energy * (1.0f - _LoudnessAlpha);
			_OutputEnvelope = _OutputEnvelope * _LoudnessAlpha + output_energy * (1.0f - _LoudnessAlpha);

			//Calculate the difference (in decibels).
			const float32 difference{ Audio::GainToDecibels(_InputEnvelope) - Audio::GainToDecibels(_OutputEnvelope) };

			//Update the compensation.
			_Compensation = _Compensation * _CompensationAlpha + difference * (1.0f - _CompensationAlpha);

			//Calculate the gain.
			const float32 gain{ Audio::DecibelsToGain(_Compensation) };

			//Apply the gain.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				outputs->At(channel_index).At(sample_index) *= gain;
			}
		}
	}

private:

	/*
	*	Perceptual loudness weighting class definition.
	*/
	class PerceptualLoudnessWeighting final
	{

	public:

		//The high pass filter.
		HighPassFilter _HighPassFilter{ 60.0f, 1.0f, 2 };

		//The high shelf filter.
		HighShelfFilter _HighShelfFilter{ 4'000.0f, Audio::DecibelsToGain(4.0f) };

	};

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The input perceptual loudness weighting.
	PerceptualLoudnessWeighting _InputPerceptualLoudnessWeighting;

	//The output buffer.
	DynamicArray<DynamicArray<float32>> _OutputBuffer;

	//The output perceptual loudness weighting.
	PerceptualLoudnessWeighting _OutputPerceptualLoudnessWeighting;

	//The loudness alpha.
	float32 _LoudnessAlpha;

	//The input envelope.
	float32 _InputEnvelope{ 0.0f };

	//The output envelope.
	float32 _OutputEnvelope{ 0.0f };

	//The compensation alpha.
	float32 _CompensationAlpha;

	//The compensation.
	float32 _Compensation{ 0.0f };

};