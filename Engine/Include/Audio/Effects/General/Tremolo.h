#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Simple reverb audio effect.
*/
class Tremolo final : public AudioEffect
{

public:

	//Enumeration covering all times.
	enum class Time : uint32
	{
		WHOLE_DOTTED,
		WHOLE_REGULAR,
		WHOLE_TRIPLET,
		HALF_DOTTED,
		HALF_REGULAR,
		HALF_TRIPLET,
		QUARTER_DOTTED,
		QUARTER_REGULAR,
		QUARTER_TRIPLET,
		EIGHTH_DOTTED,
		EIGHTH_REGULAR,
		EIGHTH_TRIPLET,
		SIXTEENTH_DOTTED,
		SIXTEENTH_REGULAR,
		SIXTEENTH_TRIPLET,
		THIRTYSECOND_DOTTED,
		THIRTYSECOND_REGULAR,
		THIRTYSECOND_TRIPLET,
		SIXTYFOURTH_DOTTED,
		SIXTYFOURTH_REGULAR,
		SIXTYFOURTH_TRIPLET,
		HUNDREDTWENTYEIGHTH_DOTTED,
		HUNDREDTWENTYEIGHTH_REGULAR,
		HUNDREDTWENTYEIGHTH_TRIPLET,

		NUMBER_OF_TIMES
	};

	//Constants.
	constexpr static Time DEFAULT_TIME{ Time::QUARTER_REGULAR };
	constexpr static float32 DEFAULT_SHAPE{ 0.0f };
	constexpr static float32 DEFAULT_DEPTH{ 1.0f };

	//The time.
	Time _Time{ DEFAULT_TIME };

	//The shape.
	float32 _Shape{ DEFAULT_SHAPE };

	//The depth.
	float32 _Depth{ DEFAULT_DEPTH };

	//The power.
	bool _Power{ true };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Tremolo() NOEXCEPT
	{
		//Calculate the increment per sample.
		CalculateIncrementPerSample();

		//Reset the phases.
		Memory::Set(_CurrentPhases.Data(), 0, _CurrentPhases.Size() * sizeof(float32));
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Calculate the increment per sample.
		CalculateIncrementPerSample();
	}

	/*
	*	Callback for when the beats per minute changed.
	*/
	FORCE_INLINE void OnBeatsPerMinuteChanged() NOEXCEPT override
	{
		//Calculate the increment per sample.
		CalculateIncrementPerSample();
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
		//If the power isn't on, copy the inputs into the outputs and move in.
		if (!_Power)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
			}

			return;
		}

		//Check if the time changed.
		if (_Time != _PreviousTime)
		{
			CalculateIncrementPerSample();
			_PreviousTime = _Time;
		}

		//Calculate the shape properties.
		const float32 K{ std::exp(_Shape * 4.0f) };
		const float32 shape_normalizer{ 1.0f / std::tanh(K) };

		//Iterate over all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Cache the current phase.
			float32 &current_phase{ _CurrentPhases[channel_index] };

			//Reset the phase if the timeline changed.
			current_phase *= static_cast<float32>(context._WasTimelineRunning == context._IsTimelineRunning);

			//Process the samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Increment the current phase.
				current_phase += _IncrementPerSample;

				//Wrap around.
				current_phase -= BaseMathConstants::DOUBLE_PI * static_cast<float32>(current_phase >= BaseMathConstants::DOUBLE_PI);

				//Calculate the phase offset.
				const float32 phase_offset{ BaseMath::LinearlyInterpolate(BaseMathConstants::HALF_PI, 0.0f, _Shape) };

				//Calculate the multiplier.
				float32 multiplier{ std::sin(current_phase + phase_offset) };

				//Apply the shape.
				multiplier = std::tanh(multiplier * K) * shape_normalizer;

				//Normalize.
				multiplier = multiplier * 0.5f + 0.5f;

				//Apply the depth.
				multiplier = BaseMath::LinearlyInterpolate(1.0f, multiplier, _Depth);

				//Apply the multiplier!
				outputs->At(channel_index).At(sample_index) = inputs.At(channel_index).At(sample_index) * multiplier;
			}
		}
	}

	/*
	*	Returns the time names.
	*/
	FORCE_INLINE static NO_DISCARD ArrayProxy<const char*> GetTimeNames() NOEXCEPT
	{
		static const char *TIME_NAMES[]
		{
			"1/1 D",
			"1/1",
			"1/1 T",

			"1/2 D",
			"1/2",
			"1/2 T",

			"1/4 D",
			"1/4",
			"1/4 T",

			"1/8 D",
			"1/8",
			"1/8 T",

			"1/16 D",
			"1/16",
			"1/16 T",

			"1/32 D",
			"1/32",
			"1/32 T",

			"1/64 D",
			"1/64",
			"1/64 T",

			"1/128 D",
			"1/128",
			"1/128 T"
		};

		return ArrayProxy<const char*>(&TIME_NAMES[0], ARRAY_LENGTH(TIME_NAMES));
	}

private:

	//The previous time.
	Time _PreviousTime{ DEFAULT_TIME };

	//The increment per sample..
	float32 _IncrementPerSample{ 0.0f };

	//The current phases.
	StaticArray<float32, 2> _CurrentPhases;

	/*
	*	Calculates the tremolo time.
	*/
	FORCE_INLINE NO_DISCARD float32 TremoloTime() const NOEXCEPT
	{
		//Calculate the duration and modifier.
		const Audio::NoteDuration duration{ static_cast<Audio::NoteDuration>(static_cast<uint32>(_Time) / 3) };
		Audio::NoteModifier modifier;

		switch (static_cast<uint32>(_Time) % 3)
		{
			case 0:
			{
				modifier = Audio::NoteModifier::DOTTED;

				break;
			}

			case 1:
			{
				modifier = Audio::NoteModifier::NONE;

				break;
			}

			case 2:
			{
				modifier = Audio::NoteModifier::TRIPLET;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				modifier = Audio::NoteModifier::NONE;

				break;
			}
		}

		//Calculate the time!
		return Audio::CalculateNoteTime(_BeatsPerMinute, duration, modifier);
	}

	/*
	*	Calculates the increment per sample.
	*/
	FORCE_INLINE void CalculateIncrementPerSample() NOEXCEPT
	{
		//Calculate the increment per sample.
		_IncrementPerSample = (1.0f / TremoloTime()) * BaseMathConstants::DOUBLE_PI / _SampleRate;
	}

};