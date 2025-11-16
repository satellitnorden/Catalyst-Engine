#pragma once

#define PITCH_SHIFT_ALGORITHM_SMB (0)
#define PITCH_SHIFT_ALGORITHM_1 (1)
#define PITCH_SHIFT_ALGORITHM_2 (2)

#define PITCH_SHIFT_ALGORITHM PITCH_SHIFT_ALGORITHM_SMB

#if PITCH_SHIFT_ALGORITHM == PITCH_SHIFT_ALGORITHM_SMB
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
			smbPitchShift::smbPitchShift(pitch_shift, number_of_samples, 2048, 16, _SampleRate, _InputBuffers[channel_index].Data(), outputs->At(channel_index).Data(), &_States[channel_index]);
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
#elif PITCH_SHIFT_ALGORITHM == PITCH_SHIFT_ALGORITHM_1
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
#elif PITCH_SHIFT_ALGORITHM == PITCH_SHIFT_ALGORITHM_2
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
		//Calculate the read speed.
		const float32 read_speed{ ReadSpeed(TotalShift()) };

		//Process all samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Process all channels.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				//Cache channel-specific data.
				DynamicArray<float32> &buffer{ _Buffers[channel_index] };

				//Write into the buffer.
				buffer[_WritePosition] = inputs.At(channel_index).At(sample_index);

				//Sample the pitched sample.
				float32 pitched_sample;

				{
					//Calculate the grain start blend sample.
					const uint32 grain_start_blend_sample{ static_cast<uint32>(static_cast<float32>(_GrainSamples) * (1.0f - FADE_FRACTION)) };

					//Should we be blending?
					const bool should_be_blending{ _GrainProgress > grain_start_blend_sample };

					//If we should be blending, but are not, set up the blend data.
					if (should_be_blending && !_IsBlending)
					{
						_BlendReadPosition = CalculateStartReadPosition(_WritePosition);

						_IsBlending = true;
					}

					//If we are blending, sample both at the read position and the blend read position and blend.
					if (_IsBlending)
					{
						const float32 current_pitched_sample{ ReadBuffer(buffer, _ReadPosition) };
						const float32 blend_pitched_sample{ ReadBuffer(buffer, _BlendReadPosition) };
						const float32 alpha{ static_cast<float32>(_GrainProgress - grain_start_blend_sample) / static_cast<float32>(_GrainSamples - grain_start_blend_sample) };

						//pitched_sample = Audio::Crossfade(current_pitched_sample, blend_pitched_sample, alpha);
						pitched_sample = BaseMath::LinearlyInterpolate(current_pitched_sample, blend_pitched_sample, alpha);
					}

					//Otherwise, just read the buffer.
					else
					{
						pitched_sample = ReadBuffer(buffer, _ReadPosition);
					}
				}

				//Write the output sample.
				outputs->At(channel_index).At(sample_index) = pitched_sample;
			}

			//Increment the write position.
			++_WritePosition;
			_WritePosition *= static_cast<uint32>(_WritePosition < static_cast<uint32>(BUFFER_SIZE));

			//Increment the read position.
			_ReadPosition += read_speed;
			_ReadPosition -= static_cast<float32>(BUFFER_SIZE) * static_cast<float32>(_ReadPosition >= static_cast<float32>(BUFFER_SIZE));
			
			//Increment the grain progress.
			++_GrainProgress;

			//Update the blend read position.
			_BlendReadPosition += read_speed;
			_BlendReadPosition -= static_cast<float32>(BUFFER_SIZE) * static_cast<float32>(_BlendReadPosition >= static_cast<float32>(BUFFER_SIZE));

			//Has this grain reached it's end?
			if (_GrainProgress >= _GrainSamples)
			{
				_GrainProgress = 0;
				_GrainSamples = Audio::TimeToSamples(GrainTime(TotalShift()), _SampleRate);
				_ReadPosition = _BlendReadPosition;
				_IsBlending = false;
			}
		}
	}

private:

	//Constants.
	constexpr static uint64 BUFFER_SIZE{ 1'024 };
	constexpr static float32 FADE_FRACTION{ 0.4'75f };

	//The buffers.
	StaticArray<DynamicArray<float32>, 2> _Buffers;

	//The write position.
	uint32 _WritePosition;

	//The read position.
	float32 _ReadPosition;

	//The grain progress.
	uint32 _GrainProgress;

	//The grain samples.
	uint32 _GrainSamples;

	//Denotes whether or not we are blending.
	bool _IsBlending;

	//The blend read position.
	float32 _BlendReadPosition;

	/*
	*	Resets
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		//Set up the buffers.
		for (DynamicArray<float32> &buffer : _Buffers)
		{
			buffer.Upsize<false>(BUFFER_SIZE);
			Memory::Set(buffer.Data(), 0, sizeof(float32) * buffer.Size());
		}

		//Reset the write position.
		_WritePosition = 0;

		//Reset the read position.
		_ReadPosition = 0.0f;

		//Reset the grain progress.
		_GrainProgress = 0;

		//Calculate the grain samples.
		_GrainSamples = Audio::TimeToSamples(GrainTime(TotalShift()), _SampleRate);

		//Reset whether or not we are blending.
		_IsBlending = false;
	}

	/*
	*	Returns the total shift.
	*/
	FORCE_INLINE NO_DISCARD float32 TotalShift() const NOEXCEPT
	{
		return static_cast<float32>(_Coarse) + _Fine;
	}

	/*
	*	Calculates the read speed.
	*/
	FORCE_INLINE NO_DISCARD float32 ReadSpeed(const float32 total_shift) const NOEXCEPT
	{
		return std::pow(2.0f, total_shift * (1.0f / 12.0f));
	}

	/*
	*	Calculates the start read position.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculateStartReadPosition(const uint32 write_position) NOEXCEPT
	{
		//Calculate the start read position.
		float32 start_read_position{ static_cast<float32>(write_position) };

		//Retrieve the total shift.
		const float32 total_shift{ TotalShift() };

		//If we're down-pitching, set the start read position as close to the write position as possible, as it won't catch up.
		if (total_shift < 0.0f)
		{
			start_read_position = static_cast<float32>(write_position) - 1.0f;
		}

		//Otherwise, set the start read position so that it will catch up to the write position at the end of the grain.
		else if (total_shift > 0.0f)
		{
			//Calculate the grain time.
			const float32 grain_time{ GrainTime(total_shift) * (1.0f + FADE_FRACTION) };

			//Calculate the grain samples.
			const uint32 grain_samples{ Audio::TimeToSamples(grain_time, _SampleRate) + 1 };

			//Calculate where the write position will be in the grain time.
			const float32 future_write_position{ static_cast<float32>(write_position + grain_samples) };

			//Offset the start read position so that it is behind the future write position by grain samples * read speed.
			start_read_position = future_write_position - (static_cast<float32>(grain_samples) * ReadSpeed(total_shift));
		}

		//Ensure the start read position is within the buffer extents.
		while (start_read_position < 0.0f)
		{
			start_read_position += static_cast<float32>(BUFFER_SIZE);
		}

		while (start_read_position >= static_cast<float32>(BUFFER_SIZE))
		{
			start_read_position -= static_cast<float32>(BUFFER_SIZE);
		}

		//Return the start read position!
		return start_read_position;
	}

	/*
	*	Returns the grain time for the given total shift.
	*/
	FORCE_INLINE NO_DISCARD float32 GrainTime(const float32 total_shift) const NOEXCEPT
	{
		//Define constants.
		constexpr float32 MINIMUM_GRAIN_TIME{ 5.0f / 1'000.0f };
		constexpr float32 MAXIMUM_GRAIN_TIME{ 20.0f / 1'000.0f };

		//Calculate the normalized shift.
		const float32 normalized_shift{ total_shift * (1.0f / 24.0f) };

		//Calculate the grain time!
		return BaseMath::LinearlyInterpolate(MINIMUM_GRAIN_TIME, MAXIMUM_GRAIN_TIME, BaseMath::Absolute(normalized_shift));
	}

	/*
	*	Reads the buffer at the specified position.
	*/
	FORCE_INLINE NO_DISCARD float32 ReadBuffer(const DynamicArray<float32> &buffer, float32 position) NOEXCEPT
	{
		position += static_cast<float32>(buffer.Size()) * static_cast<float32>(position < 0.0f);
		const uint32 index_0{ static_cast<uint32>(position) };
		uint32 index_1{ index_0 + 1 };
		index_1 *= static_cast<uint32>(index_1 < static_cast<uint32>(buffer.Size()));
		const float32 alpha{ BaseMath::Fractional(position) };


 		return BaseMath::LinearlyInterpolate(buffer[index_0], buffer[index_1], alpha);
	}

};
#endif