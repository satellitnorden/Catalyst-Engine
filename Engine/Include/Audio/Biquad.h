#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/BaseMath.h>

//Sound.
#include <Sound/SoundUtilities.h>

//STD.
#include <cmath>

/*
*	Generic biquad class, that can be used to set up filters, such as high/low pass filters or peak filters.
*/
class Biquad final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Biquad() NOEXCEPT
	{
		//Reset the input/output history.
		Memory::Set(_InputHistory.Data(), 0, sizeof(float32) * _InputHistory.Size());
		Memory::Set(_OutputHistory.Data(), 0, sizeof(float32) * _OutputHistory.Size());
	}

	/*
	*	Calculates the coefficients of this biquad for a high pass filter.
	*	Note that 'gain' is expected to be linear gain.
	*/
	FORCE_INLINE void InitializeHighPass(const float32 frequency, const float32 quality, const float32 sample_rate) NOEXCEPT
	{
		const float32 omega_C{ 2.0f * BaseMathConstants::PI * frequency / sample_rate };
		const float32 alpha{ BaseMath::Sine(omega_C) / (2.0f * quality) };
		const float32 cosine_omega_C{ BaseMath::Cosine(omega_C) };

		const float32 A0{ 1.0f + alpha };
		const float32 A1{ -2.0f * cosine_omega_C };
		const float32 A2{ 1.0f - alpha };
		const float32 B0{ (1.0f + cosine_omega_C) / 2.0f };
		const float32 B1{ -(1.0f + cosine_omega_C) };
		const float32 B2{ (1.0f + cosine_omega_C) / 2.0f };

		const float32 A0_reciprocal{ 1.0f / A0 };

		_A0 = A0;
		_A1 = A1 * A0_reciprocal;
		_A2 = A2 * A0_reciprocal;
		_B0 = B0 * A0_reciprocal;
		_B1 = B1 * A0_reciprocal;
		_B2 = B2 * A0_reciprocal;
	}

	/*
	*	Calculates the coefficients of this biquad for a low shelf filter.
	*	Note that 'gain' is expected to be linear gain.
	*/
	FORCE_INLINE void InitializeLowShelf(const float32 frequency, const float32 gain, const float32 sample_rate) NOEXCEPT
	{
		//This formula expects a different dB -> linear gain mapping, so calculate that.
		const float32 _gain{ std::pow(10.0f, SoundUtilities::GainToDecibels(gain) / 40.0f) };

		const float32 W0{ 2.0f * 3.1415926535897932384626433832795028841971f * (frequency / sample_rate) };
		const float32 cosine{ std::cos(W0) };
		const float32 sine{ std::sin(W0) };
		const float32 AL{ sine / 2.0f * std::sqrt((_gain + 1.0f / _gain) * (1.0f / 1.0f - 1.0f) + 2.0f) };
		const float32 square_root{ 2.0f * std::sqrt(_gain) * AL };

		const float32 A0{ (_gain + 1.0f) + (_gain - 1.0f) * cosine + square_root };
		const float32 A1{ -2.0f * ((_gain - 1.0f) + (_gain + 1.0f) * cosine) };
		const float32 A2{ (_gain + 1.0f) + (_gain - 1.0f) * cosine - square_root };
		const float32 B0{ _gain * ((_gain + 1.0f) - (_gain - 1.0f) * cosine + square_root) };
		const float32 B1{ 2.0f * _gain * ((_gain - 1.0f) - (_gain + 1.0f) * cosine) };
		const float32 B2{ _gain * ((_gain + 1.0f) - (_gain - 1.0f) * cosine - square_root) };

		const float32 A0_reciprocal{ 1.0f / A0 };

		_A0 = A0;
		_A1 = A1 * A0_reciprocal;
		_A2 = A2 * A0_reciprocal;
		_B0 = B0 * A0_reciprocal;
		_B1 = B1 * A0_reciprocal;
		_B2 = B2 * A0_reciprocal;
	}

	/*
	*	Calculates the coefficients of this biquad for a peak filter.
	*	Note that 'gain' is expected to be linear gain.
	*/
	FORCE_INLINE void InitializePeak(const float32 frequency, const float32 gain, const float32 quality, const float32 sample_rate) NOEXCEPT
	{
		//This formula expects a bandwidth parameter and a different dB -> linear gain mapping, so calculate that.
		const float32 bandwidth{ SoundUtilities::QualityToBandwidth(quality) };
		const float32 _gain{ std::pow(10.0f, SoundUtilities::GainToDecibels(gain) / 40.0f) };

		const float32 W0{ 2.0f * 3.1415926535897932384626433832795028841971f * (frequency / sample_rate) };
		const float32 cosine{ std::cos(W0) };
		const float32 sine{ std::sin(W0) };
		const float32 AL{ sine * std::sinh(0.69314718055994530941723212145818f * 0.5f * bandwidth * W0 / sine) };

		const float32 A0{ 1.0f + AL / _gain };
		const float32 A1{ -2.0f * cosine };
		const float32 A2{ 1.0f - AL / _gain };
		const float32 B0{ 1.0f + AL * _gain };
		const float32 B1{ -2.0f * cosine };
		const float32 B2{ 1.0f - AL * _gain };

		const float32 A0_reciprocal{ 1.0f / A0 };

		_A0 = A0;
		_A1 = A1 * A0_reciprocal;
		_A2 = A2 * A0_reciprocal;
		_B0 = B0 * A0_reciprocal;
		_B1 = B1 * A0_reciprocal;
		_B2 = B2 * A0_reciprocal;
	}

	/*
	*	Calculates the coefficients of this biquad for a high shelf filter.
	*	Note that 'gain' is expected to be linear gain.
	*/
	FORCE_INLINE void InitializeHighShelf(const float32 frequency, const float32 gain, const float32 sample_rate) NOEXCEPT
	{
		//This formula expects a different dB -> linear gain mapping, so calculate that.
		const float32 _gain{ std::pow(10.0f, SoundUtilities::GainToDecibels(gain) / 40.0f) };

		const float32 W0{ 2.0f * 3.1415926535897932384626433832795028841971f * (frequency / sample_rate) };
		const float32 cosine{ std::cos(W0) };
		const float32 sine{ std::sin(W0) };
		const float32 AL{ sine / 2.0f * std::sqrt((_gain + 1.0f / _gain) * (1.0f / 1.0f - 1.0f) + 2.0f) };
		const float32 square_root{ 2.0f * std::sqrt(_gain) * AL };

		const float32 A0{ (_gain + 1.0f) - (_gain - 1.0f) * cosine + square_root };
		const float32 A1{ 2.0f * ((_gain - 1.0f) - (_gain + 1.0f) * cosine) };
		const float32 A2{ (_gain + 1.0f) - (_gain - 1.0f) * cosine - square_root };
		const float32 B0{ _gain * ((_gain + 1.0f) + (_gain - 1.0f) * cosine + square_root) };
		const float32 B1{ -2.0f * _gain * ((_gain - 1.0f) + (_gain + 1.0f) * cosine) };
		const float32 B2{ _gain * ((_gain + 1.0f) + (_gain - 1.0f) * cosine - square_root) };

		const float32 A0_reciprocal{ 1.0f / A0 };

		_A0 = A0;
		_A1 = A1 * A0_reciprocal;
		_A2 = A2 * A0_reciprocal;
		_B0 = B0 * A0_reciprocal;
		_B1 = B1 * A0_reciprocal;
		_B2 = B2 * A0_reciprocal;
	}

	/*
	*	Calculates the coefficients of this biquad for a low pass filter.
	*	Note that 'gain' is expected to be linear gain.
	*/
	FORCE_INLINE void InitializeLowPass(const float32 frequency, const float32 quality, const float32 sample_rate) NOEXCEPT
	{
		const float32 omega_C{ 2.0f * BaseMathConstants::PI * frequency / sample_rate };
		const float32 alpha{ BaseMath::Sine(omega_C) / (2.0f * quality) };
		const float32 cosine_omega_C{ BaseMath::Cosine(omega_C) };

		const float32 A0{ 1.0f + alpha };
		const float32 A1{ -2.0f * cosine_omega_C };
		const float32 A2{ 1.0f - alpha };
		const float32 B0{ (1.0f - cosine_omega_C) / 2.0f };
		const float32 B1{ 1.0f - cosine_omega_C };
		const float32 B2{ (1.0f - cosine_omega_C) / 2.0f };

		const float32 A0_reciprocal{ 1.0f / A0 };

		_A0 = A0;
		_A1 = A1 * A0_reciprocal;
		_A2 = A2 * A0_reciprocal;
		_B0 = B0 * A0_reciprocal;
		_B1 = B1 * A0_reciprocal;
		_B2 = B2 * A0_reciprocal;
	}

	/*
	*	Processes this biquad filter.
	*/
	FORCE_INLINE void Process(const float32 *const RESTRICT inputs, float32 *const RESTRICT outputs, const uint32 number_of_samples) NOEXCEPT
	{
		float32 X1{ _InputHistory[0] };
		float32 X2{ _InputHistory[1] };
		float32 Y1{ _OutputHistory[0] };
		float32 Y2{ _OutputHistory[1] };

		for (uint64 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			const float32 X0{ inputs[sample_index] };

			const float32 Y0
			{
				_B0 * X0
				+ _B1 * X1
				+ _B2 * X2
				- _A1 * Y1
				- _A2 * Y2
			};

			outputs[sample_index] = Y0;

			X2 = X1;
			X1 = X0;
			Y2 = Y1;
			Y1 = Y0;
		}

		_InputHistory[0] = X1;
		_InputHistory[1] = X2;
		_OutputHistory[0] = Y1;
		_OutputHistory[1] = Y2;
	}

private:

	//The input history.
	StaticArray<float32, 2> _InputHistory;

	//The output history.
	StaticArray<float32, 2> _OutputHistory;

	//The coefficients.
	float32 _A0{ 1.0f };
	float32 _A1{ 0.0f };
	float32 _A2{ 0.0f };
	float32 _B0{ 1.0f };
	float32 _B1{ 0.0f };
	float32 _B2{ 0.0f };

};