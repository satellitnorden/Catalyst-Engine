#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/SmoothParameter.h>

//Third party.
#include <NAM/extensions/parametric_lstm.h>
#include <NAM/extensions/parametric_wavenet.h>
#include <NAM/get_dsp.h>
#include <NAM/lstm.h>

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
		//The initialization order may be a bit whack here, so make sure that architectures are registered.
		nam::parametric_lstm::RegisterFactory();
		nam::parametric_wavenet::RegisterFactory();
		nam::lstm::RegisterFactory();

		//Retrieve the DSP's.
		for (std::unique_ptr<nam::DSP> &dsp : _DSPs)
		{
			nam::dspData dsp_data;
			dsp = nam::get_dsp(std::filesystem::path(std::string(file_path)), dsp_data);
			_InputSize = input_size;
		}

		//Now that we know the input size, set up the parameters.
		if (_InputSize > 1)
		{
			_Parameters.Upsize<true>(_InputSize - 1);
		}

		//Set the gain compensation.
		_GainCompensation = gain_compensation;
	}

	/*
	*	Sets the parameter value at the given index.
	*/
	FORCE_INLINE void SetParameter(const uint64 index, const float32 value) NOEXCEPT
	{
		if (index < _Parameters.Size())
		{
			_Parameters[index].SetTarget(value);
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
		//Fill up the input buffers.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_InputBuffers[channel_index].Resize<false>(number_of_samples * _InputSize);
		}

		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				_InputBuffers[channel_index][sample_index * _InputSize + 0] = inputs.At(channel_index).At(sample_index);

				for (uint64 parameter_index{ 0 }; parameter_index < _Parameters.Size(); ++parameter_index)
				{
					_InputBuffers[channel_index][sample_index * _InputSize + 1 + parameter_index] = _Parameters[parameter_index].GetCurrent();
				}
			}

			//Update the parameters.
			for (SmoothParameter &parameter : _Parameters)
			{
				parameter.UpdateCurrent();
			}
		}
		//Process each channel.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Process the DSP!
			_DSPs[channel_index]->process(_InputBuffers[channel_index].Data(), outputs->At(channel_index).Data(), number_of_samples);
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

	//The DSP's.
	StaticArray<std::unique_ptr<nam::DSP>, 2> _DSPs;

	//The parameters.
	DynamicArray<SmoothParameter> _Parameters;

	//The input size.
	uint32 _InputSize;

	//The input buffers.
	StaticArray<DynamicArray<float32>, 2> _InputBuffers;

	/*
	*	The high pass filter.
	*	This is implemented as a post-processing step to remove the DC offset that may be inherent in some models.
	*/
	HighPassFilter _HighPassFilter{ 5.0f, 1.0f, 2 };

	//The gain compensation.
	float32 _GainCompensation;

};