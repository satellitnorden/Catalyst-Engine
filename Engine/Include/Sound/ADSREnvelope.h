#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

class ADSREnvelope
{

public:

  //Enumeration covering all stages.
  enum class Stage : uint8
  {
    STAGE_OFF,
    STAGE_ATTACK,
    STAGE_DECAY,
    STAGE_SUSTAIN,
    STAGE_RELEASE
  };

  /*
  * Default constructor.
  */
  ADSREnvelope()
  {
    _StageValues[0] = 0.001f; //ATTACK TIME
    _StageValues[1] = 0.01f; //DECAY TIME
    _StageValues[2] = 0.9f; //SUSTAIN GAIN
    _StageValues[3] = 0.01f; //RELEASE TIME
  }

  /*
  * Increments the current sample.
  */
  FORCE_INLINE void IncrementCurrentSample() NOEXCEPT
  {
    ++_CurrentSample;
  }

  /*
  * Sets the sample rate.
  */
  FORCE_INLINE void SetSampleRate(const float sample_rate)
  {
    _SampleRate = sample_rate;
  }

  /*
  * Sets the stage values.
  */
  FORCE_INLINE void SetStageValues(const float attack, const float decay, const float sustain, const float release)
  {
    _StageValues[0] = attack;
    _StageValues[1] = decay;
    _StageValues[2] = sustain;
    _StageValues[3] = release;
  }

  /*
  * Returns if this envelope is active.
  */
  FORCE_INLINE bool IsActive()
  {
    return _CurrentStage != Stage::STAGE_OFF;
  }

  /*
  * Enters the OFF stage for this envelope.
  */
  FORCE_INLINE void EnterOffStage()
  {
    _CurrentStage = Stage::STAGE_OFF;
    _CurrentSample = 0;
    _SamplesUntilNextStage = 0;
  }

  /*
  * Enters the ATTACK stage for this envelope.
  */
  FORCE_INLINE void EnterAttackStage()
  {
    _CurrentStage = Stage::STAGE_ATTACK;
    _CurrentSample = 0;
    _SamplesUntilNextStage = static_cast<uint32>(_StageValues[0] * _SampleRate);
  }

  /*
  * Enters the DECAY stage for this envelope.
  */
  FORCE_INLINE void EnterDecayStage()
  {
    _CurrentStage = Stage::STAGE_DECAY;
    _CurrentSample = 0;
    _SamplesUntilNextStage = static_cast<uint32>(_StageValues[1] * _SampleRate);
  }

  /*
  * Enters the SUSTAIN stage for this envelope.
  */
  FORCE_INLINE void EnterSustainStage()
  {
    _CurrentStage = Stage::STAGE_SUSTAIN;
    _CurrentSample = 0;
    _SamplesUntilNextStage = 0;
  }

  /*
  * Enters the RELEASE stage for this envelope.
  */
  FORCE_INLINE void EnterReleaseStage()
  {
    _CurrentStage = Stage::STAGE_RELEASE;
    _CurrentSample = 0;
    _SamplesUntilNextStage = static_cast<uint32>(_StageValues[3] * _SampleRate);
  }

  /*
  * Updates this envelope, returning the current gain.
  */
  FORCE_INLINE float Update()
  {
    switch (_CurrentStage)
    {
      case Stage::STAGE_OFF:
      {
        return 0.0f;
      }

      case Stage::STAGE_ATTACK:
      {
        //Calculate the multiplier.
        const float multiplier{ static_cast<float>(_CurrentSample) / static_cast<float>(_SamplesUntilNextStage) };

        //Check if the decay stage should be entered.
        if (_CurrentSample >= _SamplesUntilNextStage)
        {
          EnterDecayStage();
        }

        //For the attack stage, the gain is simply the multiplier.
        return multiplier;
      }

      case Stage::STAGE_DECAY:
      {
        //Calculate the multiplier.
        const float multiplier{ static_cast<float>(_CurrentSample) / static_cast<float>(_SamplesUntilNextStage) };

        //Check if the sustain stage should be entered.
        if (_CurrentSample >= _SamplesUntilNextStage)
        {
          EnterSustainStage();
        }

        //For the decay stage, blend in the sustain value.
        return CatalystBaseMath::LinearlyInterpolate(1.0f, _StageValues[2], multiplier);
      }

      case Stage::STAGE_SUSTAIN:
      {
        //For the sustain stage, simply return the sustain value.
        return _StageValues[2];
      }

      case Stage::STAGE_RELEASE:
      {
        //Calculate the multiplier.
        const float multiplier{ static_cast<float>(_CurrentSample) / static_cast<float>(_SamplesUntilNextStage) };

        //Check if the off stage should be entered.
        if (_CurrentSample >= _SamplesUntilNextStage)
        {
          EnterOffStage();
        }

        //For the decay stage, blend in the sustain value.
        return CatalystBaseMath::LinearlyInterpolate(_StageValues[2], 0.0f, multiplier);
      }

      default:
      {
        return 0.0f;
      }
    }
  }

private:

  //The current stage.
  Stage _CurrentStage{ Stage::STAGE_OFF };

  //The stage values. Represents time for ATTACK, DECAY and RELEASE, and gain for SUSTAIN.
  StaticArray<float32, 4> _StageValues;

  //The sample rate.
  float32 _SampleRate{ 48'000.0f };

  //The current sample.
  uint32 _CurrentSample{ 0 };

  //The number of samples until the next stage.
  uint32 _SamplesUntilNextStage{ 0 };

};