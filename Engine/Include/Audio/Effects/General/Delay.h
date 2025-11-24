#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/DelayLine.h>

/*
*	Simple reverb audio effect.
*/
class Delay final : public AudioEffect
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
	constexpr static float32 DEFAULT_FEEDBACK{ 0.5f };
	constexpr static float32 DEFAULT_MIX{ 0.5f };

	//The time.
	Time _Time{ DEFAULT_TIME };

	//The feedback.
	float32 _Feedback{ DEFAULT_FEEDBACK };

	//The mix.
	float32 _Mix{ DEFAULT_MIX };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Delay() NOEXCEPT
	{
		//Calculate the delay time.
		const float32 delay_time{ DelayTime() };

		//Initialize all delay lines.
		for (DelayLine &delay_line : _DelayLines)
		{
			delay_line.Initialize(delay_time, _SampleRate);
		}
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Calculate the delay time.
		const float32 delay_time{ DelayTime() };

		//Update the delay lines.
		for (DelayLine &delay_line : _DelayLines)
		{
			delay_line.Update(delay_time, _SampleRate);
		}
	}

	/*
	*	Callback for when the beats per minute changed.
	*/
	FORCE_INLINE void OnBeatsPerMinuteChanged() NOEXCEPT override
	{
		//Calculate the delay time.
		const float32 delay_time{ DelayTime() };

		//Update the delay lines.
		for (DelayLine &delay_line : _DelayLines)
		{
			delay_line.Update(delay_time, _SampleRate);
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
		//Update the delay lines, if necessary.
		if (_PreviousTime != _Time)
		{
			//Calculate the delay time.
			const float32 delay_time{ DelayTime() };

			//Update the delay lines.
			for (DelayLine &delay_line : _DelayLines)
			{
				delay_line.Update(delay_time, _SampleRate);
			}

			_PreviousTime = _Time;
		}

		//Process all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Cache the delay line.
			DelayLine &delay_line{ _DelayLines[channel_index] };

			//Process all samples.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Cache the input sample.
				const float32 input_sample{ inputs.At(channel_index).At(sample_index) };

				//Retrieve the delayed sample.
				const float32 delayed_sample{ delay_line.CurrentSample() };

				//Process the delay line!
				outputs->At(channel_index).At(sample_index) = Audio::Crossfade(input_sample, delay_line.ProcessSingle(input_sample + delayed_sample * _Feedback), _Mix);
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

	//The delay lines.
	StaticArray<DelayLine, 2> _DelayLines;

	/*
	*	Calculates the delay time.
	*/
	FORCE_INLINE NO_DISCARD float32 DelayTime() const NOEXCEPT
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
};