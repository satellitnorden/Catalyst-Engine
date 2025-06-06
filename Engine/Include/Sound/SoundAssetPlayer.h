#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/SoundAsset.h>

//Sound.
#include <Sound/ADSREnvelope.h>

class SoundAssetPlayer final
{

public:

    /*
    *   Default constructor.
    */
    FORCE_INLINE SoundAssetPlayer() NOEXCEPT
    {
        //Set up the current samples.
        for (int64 &current_sample : _CurrentSamples)
        {
            current_sample = 0;
        }

        //Set up the current sample fractions.
        for (float32 &current_sample_fraction : _CurrentSampleFractions)
        {
            current_sample_fraction = 0.0f;
        }
    }

    /*
    *   Sets the sound asset.
    */
    FORCE_INLINE void SetSoundAsset(const AssetPointer<SoundAsset> sound_asset) NOEXCEPT
    {
        _SoundAsset = sound_asset;
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
        const float32 angle{ BaseMath::DegreesToRadians(45.0f) * new_pan };
        const float32 angle_sine{ BaseMath::Sine(angle) };
        const float32 angle_cosine{ BaseMath::Cosine(angle) };

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
    FORCE_INLINE void SetCurrentSample(const int64 sample, const float32 sample_fraction = 0.0f) NOEXCEPT
    {
        for (int64 &current_sample : _CurrentSamples)
        {
            current_sample = sample;
        }

        for (float32 &current_sample_fraction : _CurrentSampleFractions)
        {
            current_sample_fraction = sample_fraction;
        }
    }

    /*
    *   Returns the ADSR envelope, const.
    */
    FORCE_INLINE NO_DISCARD const ADSREnvelope &GetADSREnvelope(const uint8 channel_index) const NOEXCEPT
    {
        return _ADSREnvelopes[channel_index];
    }

    /*
    *   Returns the ADSR envelope, non-const.
    */
    FORCE_INLINE NO_DISCARD ADSREnvelope &GetADSREnvelope(const uint8 channel_index) NOEXCEPT
    {
        return _ADSREnvelopes[channel_index];
    }

    /*
    *   Advances this sound player.
    */
    FORCE_INLINE void Advance(const uint8 channel_index) NOEXCEPT
    {
        _CurrentSampleFractions[channel_index] += _PlaybackSpeed;

        while (_CurrentSampleFractions[channel_index] >= 1.0f)
        {
            ++_CurrentSamples[channel_index];
            _CurrentSampleFractions[channel_index] -= 1.0f;
        }

        if (_IsLooping && _CurrentSamples[channel_index] >= static_cast<int64>(_SoundAsset->_Samples[0].Size()))
        {
            _CurrentSamples[channel_index] = 0;
        }

        _ADSREnvelopes[channel_index].Advance();
    }

    /*
    *   Returns the next sample.
    */
    FORCE_INLINE float32 NextSample(const uint8 channel_index) NOEXCEPT
    {
        //If the playback position is before the beginning of the sound asset, just return.
        if (_CurrentSamples[channel_index] < 0)
        {
            return 0.0f;
        }

        const uint8 actual_channel_index{ static_cast<uint8>(channel_index < _SoundAsset->_Samples.Size() ? channel_index : 0) };

        if (_CurrentSamples[channel_index] < static_cast<int64>(_SoundAsset->_Samples[actual_channel_index].Size()))
        {
            const int16 first_sample{ _SoundAsset->_Samples[actual_channel_index][_CurrentSamples[channel_index]] };
            const int16 second_sample{ _SoundAsset->_Samples[actual_channel_index][_CurrentSamples[channel_index] < static_cast<int64>(_SoundAsset->_Samples[actual_channel_index].Size() - 1) ? _CurrentSamples[channel_index] + 1 : _CurrentSamples[channel_index]] };

            float32 interpolated_sample{ BaseMath::LinearlyInterpolate(static_cast<float32>(first_sample), static_cast<float32>(second_sample), _CurrentSampleFractions[channel_index]) / static_cast<float32>(INT16_MAXIMUM) };

            //Apply the pan.
            interpolated_sample *= channel_index == 0 ? _LeftPanCoefficient : _RightPanCoefficient;

            return interpolated_sample * _Gain * _ADSREnvelopes[channel_index].NextSample();
        }

        else
        {
            _IsActive = false;

            return 0.0f;
        }
    }

    /*
    *   Sets whether or not the sound asset is looping.
    */
    FORCE_INLINE void SetIsLooping(const bool is_looping) NOEXCEPT
    {
        _IsLooping = is_looping;
    }

    /*
    *   Stops the sound asset player.
    */
    FORCE_INLINE void Stop() NOEXCEPT
    {
        for (ADSREnvelope &envelope : _ADSREnvelopes)
        {
            envelope.EnterReleaseStage();
        }
    }

    /*
    *   Returns if this sound asset player is active.
    */
    FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
    {
        return _CurrentSamples[0] < 0 || (_IsActive && _ADSREnvelopes[0].IsActive());
    }

    /*
    *   Returns the current audio time.
    */
    FORCE_INLINE NO_DISCARD float64 GetCurrentAudioTime() const NOEXCEPT
    {
        return (static_cast<float64>(_CurrentSamples[0]) + static_cast<float64>(_CurrentSampleFractions[0])) / static_cast<float64>(_SoundAsset->_SampleRate);
    }

private:

    //The sound asset.
    AssetPointer<SoundAsset> _SoundAsset;

    //The gain.
    float32 _Gain{ 1.0f };

    //The left pan coefficient.
    float32 _LeftPanCoefficient{ 1.0f };

    //The right pan coefficient.
    float32 _RightPanCoefficient{ 1.0f };

    //The playback speed.
    float32 _PlaybackSpeed{ 1.0f };

    //The current samples.
    StaticArray<int64, 2> _CurrentSamples;

    //The current sample fractions.
    StaticArray<float32, 2> _CurrentSampleFractions;

    //Denotes if the sound asset is looping.
    bool _IsLooping{ false };

    //Denotes if this sound asset player is active.
    bool _IsActive{ true };

    //The ADSR envelopes.
    StaticArray<ADSREnvelope, 2> _ADSREnvelopes;

};