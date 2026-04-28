#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Core/Audio.h>

/*
*	Simple ADSR envelope class.
*/
class ADSREnvelope final
{

public:

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const float32 sample_rate) NOEXCEPT
	{
		_SampleRate = sample_rate;

		SetAttack();
	}

	/*
	*	Samples this ADSR envelope.
	*/
	FORCE_INLINE NO_DISCARD float32 Sample() const NOEXCEPT
	{
		switch (_Stage)
		{
			case Stage::ATTACK:
			{
				const float32 alpha{ static_cast<float32>(_CurrentSample) * _SampleUntilNextStageReciprocal };

				return alpha;
			}

			case Stage::DECAY:
			{
				const float32 alpha{ static_cast<float32>(_CurrentSample) * _SampleUntilNextStageReciprocal };

				return BaseMath::LinearlyInterpolate(1.0f, _SustainGain, alpha);
			}

			case Stage::SUSTAIN:
			{
				return _SustainGain;
			}

			case Stage::RELEASE:
			{
				const float32 alpha{ static_cast<float32>(_CurrentSample) * _SampleUntilNextStageReciprocal };

				return BaseMath::LinearlyInterpolate(_SustainGain, 0.0f, alpha);
			}

			case Stage::OFF:
			{
				return 0.0f;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0.0f;
			}
		}
	}

	/*
	*	Advances this ADSR envelope.
	*/
	FORCE_INLINE void Advance() NOEXCEPT
	{
		switch (_Stage)
		{
			case Stage::ATTACK:
			{
				++_CurrentSample;

				if (_CurrentSample >= _SampleUntilNextStage)
				{
					SetDecay();
				}

				break;
			}

			case Stage::DECAY:
			{
				++_CurrentSample;

				if (_CurrentSample >= _SampleUntilNextStage)
				{
					SetSustain();
				}

				break;
			}

			case Stage::SUSTAIN:
			{
				//Nothing to do here. (:

				break;
			}

			case Stage::RELEASE:
			{
				++_CurrentSample;

				if (_CurrentSample >= _SampleUntilNextStage)
				{
					SetOff();
				}

				break;
			}

			case Stage::OFF:
			{
				//Nothing to do here. (:

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Sets the stage of this ADSR envelope to ATTACK.
	*/
	FORCE_INLINE void SetAttack() NOEXCEPT
	{
		_Stage = Stage::ATTACK;
		_CurrentSample = 0;
		_SampleUntilNextStage = Audio::TimeToSamples(_AttackTime, _SampleRate);
		_SampleUntilNextStageReciprocal = 1.0f / static_cast<float32>(_SampleUntilNextStage);
	}

	/*
	*	Sets the stage of this ADSR envelope to DECAY.
	*/
	FORCE_INLINE void SetDecay() NOEXCEPT
	{
		_Stage = Stage::DECAY;
		_CurrentSample = 0;
		_SampleUntilNextStage = Audio::TimeToSamples(_DecayTime, _SampleRate);
		_SampleUntilNextStageReciprocal = 1.0f / static_cast<float32>(_SampleUntilNextStage);
	}

	/*
	*	Sets the stage of this ADSR envelope to SUSTAIN.
	*/
	FORCE_INLINE void SetSustain() NOEXCEPT
	{
		_Stage = Stage::SUSTAIN;
		_CurrentSample = 0;
		_SampleUntilNextStage = UINT32_MAXIMUM;
		_SampleUntilNextStageReciprocal = 1.0f / static_cast<float32>(_SampleUntilNextStage);
	}

	/*
	*	Sets the stage of this ADSR envelope to RELEASE.
	*/
	FORCE_INLINE void SetRelease() NOEXCEPT
	{
		_Stage = Stage::RELEASE;
		_CurrentSample = 0;
		_SampleUntilNextStage = Audio::TimeToSamples(_ReleaseTime, _SampleRate);
		_SampleUntilNextStageReciprocal = 1.0f / static_cast<float32>(_SampleUntilNextStage);
	}

	/*
	*	Sets the stage of this ADSR envelope to OFF.
	*/
	FORCE_INLINE void SetOff() NOEXCEPT
	{
		_Stage = Stage::OFF;
		_CurrentSample = 0;
		_SampleUntilNextStage = UINT32_MAXIMUM;
		_SampleUntilNextStageReciprocal = 1.0f / static_cast<float32>(_SampleUntilNextStage);
	}

	/*
	*	Returns if this ADSR envelope is off.
	*/
	FORCE_INLINE NO_DISCARD bool IsOff() const NOEXCEPT
	{
		return _Stage == Stage::OFF;
	}

private:

	//Enumeration covering all stages.
	enum class Stage : uint8
	{
		ATTACK,
		DECAY,
		SUSTAIN,
		RELEASE,

		OFF
	};

	//The sample rate.
	float32 _SampleRate{ Audio::DEFAULT_SAMPLE_RATE };

	//The stage.
	Stage _Stage{ Stage::OFF };

	//The attack time.
	float32 _AttackTime{ 1.0f / 1'000.0f };

	//The decay time.
	float32 _DecayTime{ 1.0f / 1'000.0f };

	//The sustain gain.
	float32 _SustainGain{ 1.0f };

	//The release time.
	float32 _ReleaseTime{ 2.0f / 1'000.0f };

	//The current sample.
	uint32 _CurrentSample{ 0 };

	//The sample until the next stage.
	uint32 _SampleUntilNextStage{ 0 };

	//The sample until the next stage reciprocal.
	float32 _SampleUntilNextStageReciprocal{ 0.0f };

};