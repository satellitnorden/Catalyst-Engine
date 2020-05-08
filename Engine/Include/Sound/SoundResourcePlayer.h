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
    FORCE_INLINE void SetCurrentSample(const int64 sample) NOEXCEPT
    {
        _CurrentSample = sample;
        _CurrentSampleFraction = 0.0f;
    }

    /*
    *   Advances this sound player.
    */
    FORCE_INLINE void Advance() NOEXCEPT
    {
        _CurrentSampleFraction += _PlaybackSpeed;

        while (_CurrentSampleFraction >= 1.0f)
        {
            ++_CurrentSample;
            _CurrentSampleFraction -= 1.0f;
        }
    }

    /*
    *   Returns the next sample.
    */
    FORCE_INLINE int16 NextSample(const uint64 channel) NOEXCEPT
    {
        //If the playback position is before the beginning of the sound resource, just return.
        if (_CurrentSample < 0)
        {
            return 0;
        }

        const uint64 actual_channel{ channel < _SoundResource->_Samples.Size() ? channel : 0 };

        if (_CurrentSample < static_cast<int64>(_SoundResource->_Samples[actual_channel].Size()))
        {
            const int16 first_sample{ _SoundResource->_Samples[actual_channel][_CurrentSample] };
            const int16 second_sample{ _SoundResource->_Samples[actual_channel][_CurrentSample < static_cast<int64>(_SoundResource->_Samples[actual_channel].Size() - 1) ? _CurrentSample + 1 : _CurrentSample] };

            return static_cast<int16>(CatalystBaseMath::LinearlyInterpolate(static_cast<float32>(first_sample), static_cast<float32>(second_sample), _CurrentSampleFraction));
        }

        else
        {
            _IsActive = false;

            return 0;
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
    int64 _CurrentSample{ 0 };

    //The current sample fraction.
    float32 _CurrentSampleFraction{ 0.0f };

    //Denotes if this sound resource player is active.
    bool _IsActive{ true };

};