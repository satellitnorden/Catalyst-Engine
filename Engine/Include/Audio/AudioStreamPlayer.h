#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
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
	*	Sets the gain.
	*/
	FORCE_INLINE void SetGain(const float32 value) NOEXCEPT
	{
		_Gain = value;
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
	*	Samples this audio stream player.
	*/
	FORCE_INLINE NO_DISCARD float32 Sample(const uint8 channel_index) const NOEXCEPT
	{
		if (_CurrentSample < 0)
		{
			return 0.0f;
		}
	
		const uint32 index_0{ BaseMath::Minimum<uint32>(static_cast<uint32>(_CurrentSample), _AudioStream->GetNumberOfSamples() - 1) };
		const uint32 index_1{ BaseMath::Minimum<uint32>(index_0 + 1, _AudioStream->GetNumberOfSamples() - 1) };

		return BaseMath::LinearlyInterpolate(_AudioStream->Sample(channel_index, index_0), _AudioStream->Sample(channel_index, index_1), _CurrentFraction) * _Gain;
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
	}

	/*
	*	Returns if this audio stream player is active.
	*/
	FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
	{
		return static_cast<uint32>(_CurrentSample) < _AudioStream->GetNumberOfSamples();
	}

private:

	//The audio stream.
	const AudioStream *RESTRICT _AudioStream{ nullptr };

	//The gain.
	float32 _Gain{ 1.0f };

	//The playback rate.
	float32 _PlaybackRate{ 1.0f };

	//The current sample.
	int64 _CurrentSample{ 0 };

	//The current fraction.
	float32 _CurrentFraction{ 0.0f };

};