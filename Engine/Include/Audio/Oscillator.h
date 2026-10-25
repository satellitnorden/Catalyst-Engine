#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/ADSREnvelope.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>

namespace Audio
{

	/*
	*	Simple oscillator class that generates different types of waves.
	*/
	class Oscillator final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint32
		{
			SINE,
			TRIANGLE,
			SQUARE,
			WHITE_NOISE
		};

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Oscillator() NOEXCEPT
		{
			//Enter the attack stage of the envelope immediately.
			_Envelope.SetAttack();
		}

		/*
		*	Sets the sample rate.
		*/
		FORCE_INLINE void SetSampleRate(const float32 value) NOEXCEPT
		{
			_SampleRate = value;
			_SampleRateReciprocal = 1.0f / _SampleRate;
		}

		/*
		*	Sets the type.
		*/
		FORCE_INLINE void SetType(const Type value) NOEXCEPT
		{
			_Type = value;
		}

		/*
		*	Sets the frequency.
		*/
		FORCE_INLINE void SetFrequency(const float32 value) NOEXCEPT
		{
			_Frequency = value;
		}

		/*
		*	Sets the attack time.
		*/
		FORCE_INLINE void SetAttackTime(const float32 value) NOEXCEPT
		{
			_Envelope.SetAttackTime(value);
		}

		/*
		*	Sets the decay time.
		*/
		FORCE_INLINE void SetDecayTime(const float32 value) NOEXCEPT
		{
			_Envelope.SetDecayTime(value);
		}

		/*
		*	Sets the sustain gain.
		*/
		FORCE_INLINE void SetSustainGain(const float32 value) NOEXCEPT
		{
			_Envelope.SetSustainGain(value);
		}

		/*
		*	Sets the release time.
		*/
		FORCE_INLINE void SetReleaseTime(const float32 value) NOEXCEPT
		{
			_Envelope.SetReleaseTime(value);
		}

		/*
		*	Starts this oscillator.
		*/
		FORCE_INLINE void Start() NOEXCEPT
		{
			_Envelope.SetAttack();
		}

		/*
		*	Samples this oscillator.
		*/
		FORCE_INLINE NO_DISCARD float32 Sample() NOEXCEPT
		{
			//Generate the next sample.
			float32 sample;

			switch (_Type)
			{
				case Type::SINE:
				{
					sample = std::sin(_Phase);

					break;
				}

				case Type::TRIANGLE:
				{
					const float32 phase{ _Phase / BaseMathConstants::DOUBLE_PI };
					sample = 1.0f - 4.0f * BaseMath::Absolute(phase - 0.25f);

					break;
				}

				case Type::SQUARE:
				{
					sample = _Phase >= BaseMathConstants::PI ? 1.0f : -1.0f;

					break;
				}

				case Type::WHITE_NOISE:
				{
					sample = _RandomNumberGenerator.RandomFloatInRange(-1.0f, 1.0f);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					sample = 0.0f;

					break;
				}
			}

			//Update the phase.
			_Phase += _Frequency * _SampleRateReciprocal * BaseMathConstants::DOUBLE_PI;

			//Wrap around.
			_Phase -= BaseMathConstants::DOUBLE_PI * static_cast<float32>(_Phase >= BaseMathConstants::DOUBLE_PI);

			//Apply the envelope.
			sample *= _Envelope.Sample();

			//Advance the envelope.
			_Envelope.Advance();

			//Return the sample.
			return sample;
		}

		/*
		*	Stops this oscillator.
		*/
		FORCE_INLINE void Stop() NOEXCEPT
		{
			_Envelope.SetRelease();
		}

		/*
		*	Returns if this oscillator is active.
		*/
		FORCE_INLINE NO_DISCARD bool IsActive() const NOEXCEPT
		{
			return !_Envelope.IsOff();
		}

	private:

		//The sample rate.
		float32 _SampleRate{ Audio::DEFAULT_SAMPLE_RATE };

		//The sample rate reciprocal.
		float32 _SampleRateReciprocal{ 1.0f / Audio::DEFAULT_SAMPLE_RATE };

		//The type.
		Type _Type{ Type::SINE };

		//The frequency.
		float32 _Frequency{ 1'000.0f };

		//The phase.
		float32 _Phase{ 0.0f };

		//The random number generator.
		RandomNumberGenerator _RandomNumberGenerator;

		//The envelope.
		ADSREnvelope _Envelope;

	};

}