#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>

//File.
#include <File/Core/File.h>

//Third party.
#include <ThirdParty/AudioFile/AudioFile.h>
#include <NAM/extensions/parametric_wavenet.h>
#include <NAM/wavenet/model.h>
#include <NAM/container.h>
#include <NAM/get_dsp.h>
#include <NAM/slimmable.h>

/*
*	An effect wrapping a NAM (Neural Amp Modeler) model.
*/
class NAMModel final : public AudioEffect
{

public:

	/*
	*	Initializes this NAM model.
	*/
	FORCE_INLINE void Initialize
	(
		const char *const RESTRICT file_path,
		const uint32 input_size,
		const float32 gain_compensation
	) NOEXCEPT
	{
		//If the file path doesn't exist, return.
		if (!File::Exists(file_path))
		{
			return;
		}

		//The initialization order may be a bit whack here, so make sure that architectures are registered.
		nam::container::register_parser();
		nam::parametric_wavenet::register_parser();
		nam::wavenet::register_parser();

		//Retrieve the DSP's.
		for (std::unique_ptr<nam::DSP> &dsp : _DSPs)
		{
			nam::dspData dsp_data;
			dsp = nam::get_dsp(std::filesystem::path(std::string(file_path)), dsp_data);
			dsp->Reset(static_cast<float64>(_SampleRate), 4'096);
			_InputSize = input_size;
		}

		//Now that we know the input size, set up the parameters and parameter buffers..
		if (_InputSize > 1)
		{
			_Parameters.Upsize<false>(_InputSize - 1);
			Memory::Set(_Parameters.Data(), 0, _Parameters.Size() * sizeof(float32));

			for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
			{
				_ParameterBuffers[channel_index].Upsize<true>(_InputSize - 1);
				_ParameterLowPassFilters[channel_index]._Frequency = 1.0f;
				_ParameterLowPassFilters[channel_index]._Order = 1;
			}
		}

		//Set up the input pointers.
		for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
		{
			_InputPointers[channel_index].Upsize<true>(_InputSize);
		}

		//Set the gain compensation.
		_GainCompensation = gain_compensation;

		//This NAM model is now valid!
		_Valid = true;
	}

	/*
	*	Returns if this NAM model is valid.
	*/
	FORCE_INLINE NO_DISCARD bool Valid() const NOEXCEPT
	{
		return _Valid;
	}

	/*
	*	Prewarms this NAM model.
	*/
	FORCE_INLINE NO_DISCARD void Prewarm() NOEXCEPT
	{
		//Define constants.
		constexpr uint32 NUMBER_OF_SAMPLES{ 48'000 * 1 };

		//Set up the inputs & outputs.
		DynamicArray<DynamicArray<float32>> inputs;
		DynamicArray<DynamicArray<float32>> outputs;

		inputs.Upsize<true>(2);
		outputs.Upsize<true>(2);

		for (uint8 channel_index{ 0 }; channel_index < 2; ++channel_index)
		{
			inputs.At(channel_index).Upsize<false>(NUMBER_OF_SAMPLES);
			outputs.At(channel_index).Upsize<false>(NUMBER_OF_SAMPLES);

			Memory::Set(inputs.At(channel_index).Data(), 0, NUMBER_OF_SAMPLES * sizeof(float32));
			Memory::Set(outputs.At(channel_index).Data(), 0, NUMBER_OF_SAMPLES * sizeof(float32));
		}

		//Process!
		AudioProcessContext context;

		context._WasTimelineRunning = false;
		context._IsTimelineRunning = false;

		Process(context, inputs, &outputs, 2, NUMBER_OF_SAMPLES);
	}

	/*
	*	Sets the quality.
	*/
	FORCE_INLINE void SetQuality(const float32 quality) NOEXCEPT
	{
		_CurrentQuality = quality;
	}

	/*
	*	Sets the parameter value at the given index.
	*/
	FORCE_INLINE void SetParameter(const uint64 index, const float32 value) NOEXCEPT
	{
		if (!_Valid)
		{
			return;
		}

		if (index < _Parameters.Size())
		{
			_Parameters[index] = value;
		}
	}
	
	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//TODO!
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
		if (!_Valid)
		{
			return;
		}

		//Update the quality.
		if (_PreviousQuality != _CurrentQuality)
		{
			for (std::unique_ptr<nam::DSP> &dsp : _DSPs)
			{
				if (nam::SlimmableModel *const RESTRICT slimmable_model{ dynamic_cast<nam::SlimmableModel *const RESTRICT>(dsp.get()) })
				{
					slimmable_model->SetSlimmableSize(_CurrentQuality);
				}
			}

			_PreviousQuality = _CurrentQuality;
		}

		//Fill up the input buffers.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_InputBuffers[channel_index].Resize<false>(number_of_samples);
			Memory::Copy(_InputBuffers[channel_index].Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
		}

		//Fill up the parameter buffers.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			for (uint64 parameter_index{ 0 }; parameter_index < _Parameters.Size(); ++parameter_index)
			{
				_ParameterBuffers[channel_index][parameter_index].Resize<false>(number_of_samples);
			}
		}

		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (uint64 parameter_index{ 0 }; parameter_index < _Parameters.Size(); ++parameter_index)
				{
					_ParameterBuffers[channel_index][parameter_index][sample_index] = _Parameters[parameter_index];
				}
			}
		}

		//Apply the parameters low pass filter.
		if (!_Parameters.Empty())
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				_ParameterLowPassFilters[channel_index].Process(context, _ParameterBuffers[channel_index], &_ParameterBuffers[channel_index], static_cast<uint32>(_Parameters.Size()), number_of_samples);
			}
		}

		//Process each channel.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Define constants.
			constexpr uint32 BATCH_SIZE{ 4'096 };

			//Calculate the number of batches.
			const uint32 number_of_batches{ BaseMath::Maximum<uint32>(number_of_samples / BATCH_SIZE, 1) };

			//Process each batch.
			for (uint32 batch_index{ 0 }; batch_index < number_of_batches; ++batch_index)
			{
				//Calculate the batch sample offset.
				const uint32 batch_sample_offset{ batch_index * BATCH_SIZE };

				//Calculate the number of samples for this batch.
				const uint32 number_of_samples_in_batch{ BaseMath::Minimum<uint32>(number_of_samples - batch_sample_offset, BATCH_SIZE) };

				for (uint32 input_index{ 0 }; input_index < _InputSize; ++input_index)
				{
					_InputPointers[channel_index][input_index] = input_index == 0 ? &_InputBuffers[channel_index][batch_sample_offset] : &_ParameterBuffers[channel_index][input_index - 1][batch_sample_offset];
				}

				float32 *output_pointers[]
				{
					&outputs->At(channel_index).At(batch_sample_offset)
				};

				//Process the DSP!
				_DSPs[channel_index]->process(_InputPointers[channel_index].Data(), output_pointers, number_of_samples_in_batch);
			}
		}

		//Apply the high pass filter.
		_HighPassFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

		//Apply gain compensation.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, _GainCompensation);
		}
	}

private:

	//Denotes whether or not this NAM model is valid.
	bool _Valid{ false };

	//The DSP's.
	StaticArray<std::unique_ptr<nam::DSP>, 2> _DSPs;

	//The previous quality.
	float32 _PreviousQuality{ 1.0f };

	//The current quality.
	float32 _CurrentQuality{ 1.0f };

	//The parameters.
	DynamicArray<float32> _Parameters;

	//The input size.
	uint32 _InputSize;

	//The input buffers.
	StaticArray<DynamicArray<float32>, 2> _InputBuffers;

	//The parameter buffers.
	StaticArray<DynamicArray<DynamicArray<float32>>, 2> _ParameterBuffers;

	//The parameter low pass filter.
	StaticArray<LowPassFilter, 2> _ParameterLowPassFilters;

	//The input pointers.
	StaticArray<DynamicArray<float32 *RESTRICT>, 2> _InputPointers;

	/*
	*	The high pass filter.
	*	This is implemented as a post-processing step to remove the DC offset that may be inherent in some models.
	*/
	HighPassFilter _HighPassFilter{ 5.0f, 1.0f, 2 };

	//The gain compensation.
	float32 _GainCompensation;

};