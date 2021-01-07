#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/SoundResource.h>

//Sound.
#include <Sound/ADSREnvelope.h>

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
    *   Returns the gain.
    */
    FORCE_INLINE NO_DISCARD float32 GetGain() const NOEXCEPT
    {
        return _Gain;
    }

    /*
    *   Sets the gain.
    */
    FORCE_INLINE void SetGain(const float32 new_gain) NOEXCEPT
    {
        _Gain = new_gain;
    }

    /*
    *   Sets the pan.
    */
    FORCE_INLINE void SetPan(const float32 new_pan) NOEXCEPT
    {
        //Define constants.
        const float32 angle{ CatalystBaseMath::DegreesToRadians(45.0f) * new_pan };
        const float32 angle_sine{ CatalystBaseMath::Sine(angle) };
        const float32 angle_cosine{ CatalystBaseMath::Cosine(angle) };

        _LeftPanCoefficient = angle_cosine - angle_sine;
        _RightPanCoefficient = angle_cosine + angle_sine;
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
    *   Returns the ADSR envelope, const.
    */
    FORCE_INLINE NO_DISCARD const ADSREnvelope &GetADSREnvelope() const NOEXCEPT
    {
        return _ADSREnvelope;
    }

    /*
    *   Returns the ADSR envelope, non-const.
    */
    FORCE_INLINE NO_DISCARD ADSREnvelope &GetADSREnvelope() NOEXCEPT
    {
        return _ADSREnvelope;
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

        if (_IsLooping && _CurrentSample >= static_cast<int64>(_SoundResource->_Samples[0].Size()))
        {
            _CurrentSample = 0;
        }

        _ADSREnvelope.Advance();
    }

    /*
    *   Returns the next sample.
    */
    FORCE_INLINE float32 NextSample(const uint64 channel_index) NOEXCEPT
    {
        //If the playback position is before the beginning of the sound resource, just return.
        if (_CurrentSample < 0)
        {
            return 0.0f;
        }

        const uint64 actual_channel_index{ channel_index < _SoundResource->_Samples.Size() ? channel_index : 0 };

        if (_CurrentSample < static_cast<int64>(_SoundResource->_Samples[actual_channel_index].Size()))
        {
            const int16 first_sample{ _SoundResource->_Samples[actual_channel_index][_CurrentSample] };
            const int16 second_sample{ _SoundResource->_Samples[actual_channel_index][_CurrentSample < static_cast<int64>(_SoundResource->_Samples[actual_channel_index].Size() - 1) ? _CurrentSample + 1 : _CurrentSample] };

            float32 interpolated_sample{ CatalystBaseMath::LinearlyInterpolate(static_cast<float32>(first_sample), static_cast<float32>(second_sample), _CurrentSampleFraction) / static_cast<float32>(INT16_MAXIMUM) };

            //Apply the pan.
            interpolated_sample *= channel_index == 0 ? _LeftPanCoefficient : _RightPanCoefficient;

            return interpolated_sample * _Gain * _ADSREnvelope.NextSample();
        }

        else
        {
            _IsActive = false;

            return 0.0f;
        }
    }

    /*
    *   Sets whether or not the sound resource is looping.
    */
    FORCE_INLINE void SetIsLooping(const bool is_looping) NOEXCEPT
    {
        _IsLooping = is_looping;
    }

    /*
    *   Stops the sound resource player.
    */
    FORCE_INLINE void Stop() NOEXCEPT
    {
        _ADSREnvelope.EnterReleaseStage();
    }

    /*
    *   Returns if this sound resource player is active.
    */
    FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
    {
        return _CurrentSample < 0 || (_IsActive && _ADSREnvelope.IsActive());
    }

private:

    //The sound resource.
    ResourcePointer<SoundResource> _SoundResource;

    //The gain.
    float32 _Gain{ 1.0f };

    //The left pan coefficient.
    float32 _LeftPanCoefficient{ 1.0f };

    //The right pan coefficient.
    float32 _RightPanCoefficient{ 1.0f };

    //The playback speed.
    float32 _PlaybackSpeed{ 1.0f };

    //The current sample.
    int64 _CurrentSample{ 0 };

    //The current sample fraction.
    float32 _CurrentSampleFraction{ 0.0f };

    //Denotes if the sound resource is looping.
    bool _IsLooping{ false };

    //Denotes if this sound resource player is active.
    bool _IsActive{ true };

    //The ADSR envelope.
    ADSREnvelope _ADSREnvelope;

};