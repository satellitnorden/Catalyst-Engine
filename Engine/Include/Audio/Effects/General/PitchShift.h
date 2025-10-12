#pragma once

#define USE_SMB_PITCH_SHIFT (1)

#if USE_SMB_PITCH_SHIFT
//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

//Third party.
#include <ThirdParty/smbPitchShift/smbPitchShift.h>

/*
*	This effect implements a pitch shift effect, and can be controlled by 'coarse' steps (integer semitones) and 'fine' steps (decimal semitones).
*/
class PitchShift final : public AudioEffect
{

public:

	//The coarse.
	int32 _Coarse{ 0 };

	//The fine.
	float32 _Fine{ 0.0f };

	//The mix.
	float32 _Mix{ 1.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PitchShift() NOEXCEPT
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
		//Calculate the total pitch shift.
		float32 total_pitch_shift{ (static_cast<float32>(_Coarse) + _Fine) };
		total_pitch_shift *= static_cast<float32>(total_pitch_shift < 0.01f || total_pitch_shift > 0.01f);

		//If there's no pitch shift, just copy the input into the output and return.
		if (total_pitch_shift == 0.0f)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				//Process!
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
			}

			return;
		}

		//Calculate the pitch shift.
		const float32 pitch_shift{ std::pow(2.0f, total_pitch_shift * (1.0f / 12.0f)) };

		//Copy the input into the input buffers.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			DynamicArray<float32> &input_buffer{ _InputBuffers[channel_index] };
			input_buffer.Resize<false>(number_of_samples);
			Memory::Copy(input_buffer.Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Process all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Process!
			smbPitchShift::smbPitchShift(pitch_shift, number_of_samples, 4096, 16, _SampleRate, _InputBuffers[channel_index].Data(), outputs->At(channel_index).Data(), &_States[channel_index]);
		}

		//Apply the mix.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				outputs->At(channel_index).At(sample_index) = Audio::Crossfade(_InputBuffers[channel_index][sample_index], outputs->At(channel_index).At(sample_index), _Mix);
			}
		}
	}

private:

	//The input buffers.
	StaticArray<DynamicArray<float32>, 2> _InputBuffers;

	//The states.
	StaticArray<smbPitchShift::smbPitchShiftState, 2> _States;

};
#else
//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	This effect implements a pitch shift effect, and can be controlled by 'coarse' steps (integer semitones) and 'fine' steps (decimal semitones).
*/
class PitchShift final : public AudioEffect
{

public:

	//The coarse.
	int32 _Coarse{ 0 };

	//The fine.
	float32 _Fine{ 0.0f };

	//The mix.
	float32 _Mix{ 1.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PitchShift() NOEXCEPT
	{
		//Reset.
		Reset();
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
		//Calculate the alpha.
		const float32 alpha{ std::pow(2.0f, (static_cast<float32>(_Coarse) + _Fine) / 12.0f) };

		//Put the inputs into the outputs.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Process each channel.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Cache the resample buffer.
			DynamicArray<float32> &resample_buffer{ _ResampleBuffers[channel_index] };

			//Resample!
			Resample(outputs->At(channel_index), &resample_buffer, alpha);
		}
	}

private:

	//Constants.
	constexpr static float32 FRAME_SIZE{ 10.0f / 1'000.0f };

	//The resample buffers.
	StaticArray<DynamicArray<float32>, 2> _ResampleBuffers;

	//The frame size.
	uint32 _FrameSize;

	//The frame coefficients.
	DynamicArray<float32> _FrameCoefficients;

	/*
	*	Resets
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		//Calculate the frame size.
		_FrameSize = Audio::TimeToSamples(FRAME_SIZE, _SampleRate);

		//Calculate the frame coefficients.
		_FrameCoefficients.Resize<false>(_FrameSize);

		for (uint32 sample_index{ 0 }; sample_index < _FrameSize; ++sample_index)
		{
			_FrameCoefficients[sample_index] = 0.5f - 0.5f * std::cos(2.0f * BaseMathConstants::PI * static_cast<float32>(sample_index) / static_cast<float32>(_FrameSize - 1));
		}
	}

	/*
	*	Resamples.
	*/
	FORCE_INLINE void Resample(const DynamicArray<float32> &input, DynamicArray<float32> *const RESTRICT output, const float32 alpha) NOEXCEPT
	{
		output->Clear();

		float32 current_position{ 0.0f };

		while (current_position < static_cast<float32>(input.Size()))
		{
			const uint32 index_0{ static_cast<uint32>(current_position) };
			const uint32 index_1{ BaseMath::Minimum<uint32>(index_0 + 1, input.Size() - 1) };
			const float32 blend{ BaseMath::Fractional(current_position) };

			(*output).Emplace(BaseMath::LinearlyInterpolate(input[index_0], input[index_1], blend));

			current_position += alpha;
		}
	}

	/*
	*	Time stretches.
	*/
	FORCE_INLINE void TimeStretch(const DynamicArray<float32> &input, DynamicArray<float32> *const RESTRICT output, const float32 stretch) NOEXCEPT
	{

	}

};
#endif