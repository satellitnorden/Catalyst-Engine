#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/ADSREnvelope.h>
#include <Audio/AudioStream.h>

class AudioStreamPlayer final
{

public:

	/*
	*	Returns the audio stream.
	*/
	FORCE_INLINE NO_DISCARD const AudioStream *const RESTRICT GetAudioStream() const NOEXCEPT
	{
		return _AudioStream;
	}

	/*
	*	Sets the audio stream.
	*/
	FORCE_INLINE void SetAudioStream(const AudioStream *const RESTRICT value) NOEXCEPT
	{
		_AudioStream = value;
	}

	/*
	*	Returns the ADSR envelope.
	*/
	FORCE_INLINE NO_DISCARD ADSREnvelope *const RESTRICT GetADSREnvelope() NOEXCEPT
	{
		return &_ADSREnvelope;
	}

	/*
	*	Sets the gain.
	*/
	FORCE_INLINE void SetGain(const float32 value) NOEXCEPT
	{
		_Gain = value;
	}

	/*
	*	Sets the pan.
	*/
	FORCE_INLINE void SetPan(const float32 value) NOEXCEPT
	{
		_Pan = value;

		const float32 pan_theta{ (_Pan + 1.0f) * (BaseMathConstants::PI * 0.25f) };

		_LeftPanGainMultiplier = std::cos(pan_theta);
		_RightPanGainMultiplier = std::sin(pan_theta);
	}

	/*
	*	Sets the playback rate.
	*/
	FORCE_INLINE void SetPlaybackRate(const float32 value) NOEXCEPT
	{
		_PlaybackRate = value;
	}

	/*
	*	Returns the current sample.
	*/
	FORCE_INLINE NO_DISCARD int64 GetCurrentSample() const NOEXCEPT
	{
		return _CurrentSample;
	}

	/*
	*	Sets the current sample.
	*/
	FORCE_INLINE void SetCurrentSample(const int64 value) NOEXCEPT
	{
		_CurrentSample = value;
	}

	/*
	*	Sets whether or not to loop the audio stream.
	*/
	FORCE_INLINE void SetLoop(const bool value) NOEXCEPT
	{
		_Loop = value;
	}

	/*
	*	Samples this audio stream player.
	*/
	FORCE_INLINE NO_DISCARD float32 Sample(const uint8 channel_index) const NOEXCEPT
	{
		if (_CurrentSample < 0 || !IsActive())
		{
			return 0.0f;
		}
	
		const uint32 index_0{ WrapIndex(static_cast<uint32>(_CurrentSample)) };
		const uint32 index_1{ WrapIndex(index_0 + 1) };

		return	BaseMath::LinearlyInterpolate(_AudioStream->Sample(channel_index, index_0), _AudioStream->Sample(channel_index, index_1), _CurrentFraction)
				* _Gain
				* (channel_index == 0 ? _LeftPanGainMultiplier : _RightPanGainMultiplier)
				* _ADSREnvelope.Sample();
	}

	/*
	*	Advances this audio stream player.
	*/
	FORCE_INLINE void Advance() NOEXCEPT
	{
		_CurrentFraction += _PlaybackRate;

		while (_CurrentFraction >= 1.0f)
		{
			++_CurrentSample;
			_CurrentFraction -= 1.0f;
		}

		while (_Loop && _CurrentSample >= _AudioStream->GetNumberOfSamples())
		{
			_CurrentSample -= _AudioStream->GetNumberOfSamples();
		}

		_ADSREnvelope.Advance();
	}

	/*
	*	Returns if this audio stream player is active.
	*/
	FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
	{
		return _CurrentSample < _AudioStream->GetNumberOfSamples() && !_ADSREnvelope.IsOff();
	}

private:

	//The audio stream.
	const AudioStream *RESTRICT _AudioStream{ nullptr };

	//The ADSR envelope.
	ADSREnvelope _ADSREnvelope;

	//The gain.
	float32 _Gain{ 1.0f };

	//The pan.
	float32 _Pan{ 0.0f };

	//The left pan gain multiplier.
	float32 _LeftPanGainMultiplier{ 1.0f };

	//The right pan gain multiplier.
	float32 _RightPanGainMultiplier{ 1.0f };

	//The playback rate.
	float32 _PlaybackRate{ 1.0f };

	//The current sample.
	int64 _CurrentSample{ 0 };

	//The current fraction.
	float32 _CurrentFraction{ 0.0f };

	//Denotes whether or not to loop the audio stream.
	bool _Loop{ false };

	/*
	*	Wraps the given index.
	*/
	FORCE_INLINE NO_DISCARD uint32 WrapIndex(const uint32 index) const NOEXCEPT
	{
		uint32 result{ 0 };

		result += (index - (_AudioStream->GetNumberOfSamples() * static_cast<uint32>(index >= _AudioStream->GetNumberOfSamples()))) * static_cast<uint32>(_Loop);
		result += BaseMath::Minimum<uint32>(index, _AudioStream->GetNumberOfSamples() - 1) * static_cast<uint32>(!_Loop);

		return result;
	}

};