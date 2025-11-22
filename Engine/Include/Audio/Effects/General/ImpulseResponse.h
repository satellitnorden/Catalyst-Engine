#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/SIMD.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Effects/Core/AudioEffect.h>

//Third party.
#include <ThirdParty/AudioFile/AudioFile.h>

/*
*	Class that can load and process audio signal through an impulse response.
*/
class ImpulseResponse final : public AudioEffect
{

public:

	/*
	*	Initializes this impulse response with a file path to an impulse response file.
	*/
	FORCE_INLINE void Initialize(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Construct the impulse response data.
		{
			//Load the audio file.
			AudioFile<float32> audio_file;

			if (!audio_file.load(file_path))
			{
				ASSERT(false, "Couldn't load impulse response!");

				return;
			}

			//Set the sample rate.
			_ImpulseResponseData._SampleRate = static_cast<float32>(audio_file.getSampleRate());

			//Copy the samples.
			_ImpulseResponseData._Samples.Upsize<false>(audio_file.samples[0].size());
			Memory::Copy(_ImpulseResponseData._Samples.Data(), audio_file.samples[0].data(), sizeof(float32) * audio_file.samples[0].size());
		}

		//Set up the temporary impulse response data that we will be working with.
		ImpulseResponseData impulse_response_data;

		//Resample the impulse response data.
		ResampleImpulseResponseData(&impulse_response_data);

		//Filter the impulse response data.
		FilterImpulseResponseData(&impulse_response_data);

		//Initialize using the impulse response data.
		Initialize(impulse_response_data);
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Set up the temporary impulse response data that we will be working with.
		ImpulseResponseData impulse_response_data;

		//Resample the impulse response data.
		ResampleImpulseResponseData(&impulse_response_data);

		//Filter the impulse response data.
		FilterImpulseResponseData(&impulse_response_data);

		//Initialize using the impulse response data.
		Initialize(impulse_response_data);
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
		//Process all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Cache channel data.
			DynamicArray<float32> &history{ _History[channel_index] };
			uint64 &history_index{ _HistoryIndices[channel_index] };

			//Process the samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Copy this sample into the history.
				history[history_index] = history[history_index + _RequiredHistory] = inputs.At(channel_index).At(sample_index);

				//Calculate the output.
				const float32 output{ SIMD::DotProduct(_Weights.Data(), &history[history_index], _RequiredHistory) };

				//Copy into the output.
				outputs->At(channel_index).At(sample_index) = output;

				//Update the history index.
				++history_index;
				history_index -= _RequiredHistory * static_cast<uint64>(history_index >= _RequiredHistory);
			}
		}
	}

private:

	/*
	*	Impulse response data class definition.
	*	Holds the raw loaded data, before resampling or reshuffling of weights,
	*	so that it can be kept around in case the sample rate changes.
	*/
	class ImpulseResponseData final
	{

	public:

		//The sample rate.
		float32 _SampleRate;

		//The samples.
		DynamicArray<float32> _Samples;

	};

	//Define constants.
	constexpr static uint64 REPEAT_SIZE{ 2 };

	//The impulse response data.
	ImpulseResponseData _ImpulseResponseData;

	//The weights.
	DynamicArray<float32> _Weights;

	//The required history.
	uint64 _RequiredHistory{ 0 };

	//The history.
	StaticArray<DynamicArray<float32>, 2> _History;

	//The history indices.
	StaticArray<uint64, 2> _HistoryIndices;

	/*
	*	Calculates a resampled copy of the impulse response data.
	*/
	FORCE_INLINE void ResampleImpulseResponseData(ImpulseResponseData *const RESTRICT impulse_response_data) NOEXCEPT
	{
		//If the sample rates match, just copy of the data.
		if (_ImpulseResponseData._SampleRate == _SampleRate)
		{
			impulse_response_data->_SampleRate = _ImpulseResponseData._SampleRate;
			impulse_response_data->_Samples.Upsize<false>(_ImpulseResponseData._Samples.Size());
			Memory::Copy(impulse_response_data->_Samples.Data(), _ImpulseResponseData._Samples.Data(), sizeof(float32) * _ImpulseResponseData._Samples.Size());
		}

		//Otherwise, retarget to sample rate.
		else
		{
			//Set the sample rate.
			impulse_response_data->_SampleRate = _SampleRate;

			//Calculate the ratio.
			const float32 ratio{ _ImpulseResponseData._SampleRate / _SampleRate };

			//Add samples, interpolating between samples from the original impulse response data.
			float32 current_sample{ 0.0f };

			while (static_cast<uint64>(current_sample) < _ImpulseResponseData._Samples.LastIndex())
			{
				const uint64 first_index{ static_cast<uint64>(current_sample) };
				const uint64 second_index{ first_index + 1 };
				const float32 alpha{ BaseMath::Fractional(current_sample) };

				impulse_response_data->_Samples.Emplace(BaseMath::LinearlyInterpolate(_ImpulseResponseData._Samples[first_index], _ImpulseResponseData._Samples[second_index], alpha));

				current_sample += ratio;
			}
		}
	}

	/*
	*	Filters the impulse resonse data.
	*/
	FORCE_INLINE void FilterImpulseResponseData(ImpulseResponseData *const RESTRICT impulse_response_data) NOEXCEPT
	{
		//For frequencies down to 20Hz, we'd need about ~50 milliseconds of data - Calculate how many samples that is, and then trim it.
		{
			const uint32 number_of_samples{ static_cast<uint32>((50.0f / 1'000.0f) * _SampleRate) };

			if (impulse_response_data->_Samples.Size() > number_of_samples)
			{
				impulse_response_data->_Samples.Resize<false>(number_of_samples);
			}
		}

		//Normalize it's peak to 0dB.
		{
			float32 highest_peak{ 0.0f };

			for (const float32 sample : impulse_response_data->_Samples)
			{
				highest_peak = BaseMath::Maximum<float32>(highest_peak, BaseMath::Absolute<float32>(sample));
			}

			const float32 highest_peak_reciprocal{ highest_peak > 0.0f ? 1.0f / highest_peak : 1.0f };

			for (float32 &sample : impulse_response_data->_Samples)
			{
				sample *= highest_peak_reciprocal;
			}
		}

		//Remove any samples beneath some threshold as they don't contribute to the final sound.
		{
			const float32 threshold{ Audio::DecibelsToGain(-80.0f) };

			while (BaseMath::Absolute<float32>(impulse_response_data->_Samples.Back()) < threshold)
			{
				impulse_response_data->_Samples.Pop();
			}
		}

		//Normalize to -18dB.
		{
			const float32 gain{ Audio::DecibelsToGain(-18.0f) };

			for (float32 &sample : impulse_response_data->_Samples)
			{
				sample *= gain;
			}
		}
	}

	/*
	*	Initializes with the given impulse response data.
	*/
	FORCE_INLINE void Initialize(const ImpulseResponseData &impulse_response_data) NOEXCEPT
	{
		//Set up weights.
		_Weights.Upsize<false>(impulse_response_data._Samples.Size());

		for (uint64 i{ 0 }, j{ impulse_response_data._Samples.Size() - 1 }; i < impulse_response_data._Samples.Size(); ++i, --j)
		{
			_Weights[j] = impulse_response_data._Samples[i];
		}

		//Set the required history.
		_RequiredHistory = impulse_response_data._Samples.Size();

		//Set up the history.
		for (DynamicArray<float32> &history : _History)
		{
			const uint64 required_history_size{ _RequiredHistory * REPEAT_SIZE };
			history.Upsize<false>(required_history_size);
			Memory::Set(history.Data(), 0, sizeof(float32) * history.Size());
		}

		for (uint64 &history_index : _HistoryIndices)
		{
			history_index = 0;
		}
	}

};