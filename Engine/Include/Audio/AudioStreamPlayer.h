#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

class AudioStreamPlayer final
{

public:

	/*
	*	Sets the audio stream.
	*/
	FORCE_INLINE void SetAudioStream(const AudioStream* const RESTRICT value) NOEXCEPT
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
	*	Samples this audio stream player.
	*/
	FORCE_INLINE NO_DISCARD float32 Sample(const uint8 channel_index) const NOEXCEPT
	{
		const uint32 index_0{ BaseMath::Minimum<uint32>(static_cast<uint32>(_CurrentPosition), _AudioStream->GetNumberOfSamples() - 1) };
		const uint32 index_1{ BaseMath::Minimum<uint32>(index_0 + 1, _AudioStream->GetNumberOfSamples() - 1) };
		const float32 alpha{ BaseMath::Fractional(_CurrentPosition) };

		return BaseMath::LinearlyInterpolate(_AudioStream->Sample(channel_index, index_0), _AudioStream->Sample(channel_index, index_1), alpha) * _Gain;
	}

	/*
	*	Advances this audio stream player.
	*/
	FORCE_INLINE void Advance() NOEXCEPT
	{
		_CurrentPosition += _PlaybackRate;
	}

	/*
	*	Returns if this audio stream player is active.
	*/
	FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
	{
		return static_cast<uint32>(_CurrentPosition) < _AudioStream->GetNumberOfSamples();
	}

private:

	//The audio stream.
	const AudioStream *RESTRICT _AudioStream{ nullptr };

	//The gain.
	float32 _Gain{ 1.0f };

	//The playback rate.
	float32 _PlaybackRate{ 1.0f };

	//The current position.
	float32 _CurrentPosition{ 0.0f };

};