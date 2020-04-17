#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/SoundResource.h>

class SoundResourcePlayer final
{

public:

    /*
    *   Default constructor.
    */
    FORCE_INLINE SoundResourcePlayer() NOEXCEPT
    {

    }

    /*
    *   Sets the sound resource.
    */
    FORCE_INLINE void SetSoundResource(const ResourcePointer<SoundResource> sound_resource) NOEXCEPT
    {
        _SoundResource = sound_resource;
    }

    /*
    *   Sets the gain.
    */
    FORCE_INLINE void SetGain(const float32 gain) NOEXCEPT
    {
        _Gain = gain;
    }

    /*
    *   Sets the playback speed.
    */
    FORCE_INLINE void SetPlaybackSpeed(const float32 playback_speed) NOEXCEPT
    {
        _PlaybackSpeed = playback_speed;
    }

    /*
    *   Sets the current sample.
    */
    FORCE_INLINE void SetCurrentSample(const float32 sample) NOEXCEPT
    {
        _CurrentSample = sample;
    }

    /*
    *   Advances this sound player.
    */
    FORCE_INLINE void Advance() NOEXCEPT
    {
        _CurrentSample += _PlaybackSpeed;
    }

    /*
    *   Returns the next sample.
    */
    FORCE_INLINE float32 NextSample(const uint64 channel) NOEXCEPT
    {
        const uint64 actual_channel{ channel < _SoundResource->_Samples.Size() ? channel : 0 };

        if (_CurrentSample < _SoundResource->_Samples[actual_channel].Size())
        {
            return _SoundResource->_Samples[actual_channel].Interpolate(_CurrentSample) * _Gain;
        }

        else
        {
            _IsActive = false;

            return 0.0f;
        }
    }

    /*
    *   Returns if this sound resource player is active.
    */
    FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
    {
        return _IsActive;
    }

private:

    //The sound resource.
    ResourcePointer<SoundResource> _SoundResource;

    //The gain.
    float32 _Gain{ 1.0f };

    //The playback speed.
    float32 _PlaybackSpeed{ 1.0f };

    //The current sample.
    float32 _CurrentSample{ 0.0f };

    //Denotes if this sound resource player is active.
    bool _IsActive{ true };

};