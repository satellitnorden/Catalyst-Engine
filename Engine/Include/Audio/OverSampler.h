#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/General/SIMD.h>

//Math.
#include <Math/Core/BaseMath.h>

/*
*	High-ish quality oversampler class that supports 2X or 4X oversampling.
*/
template <uint8 FACTOR, uint32 NUMBER_OF_TAPS = 64>
class OverSampler final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE OverSampler() NOEXCEPT
	{
		//Sanity check.
		ASSERT(FACTOR == 2 || FACTOR == 4, "This oversampler only supports 2X or 4X oversampling!");

		//Set up buffers.
		Memory::Set(_InputHistory.Data(), 0, sizeof(float32) * TAPS_PER_PHASE);
		Memory::Set(_PhaseBuffers.Data(), 0, sizeof(float32) * FACTOR * TAPS_PER_PHASE);
		Memory::Set(_PhaseIndices.Data(), 0, sizeof(uint32) * FACTOR);
		Memory::Set(_TapWeights.Data(), 0, sizeof(uint32) * FACTOR * TAPS_PER_PHASE);

		//Design the filters.
		DesignFilters();

		//Initialize the SIMD backend.
		SIMD::InitializeBackend();
	}

	/*
	*	Feeds this oversampler a single input sample.
	*/
	FORCE_INLINE void Feed(const float32 input_sample) NOEXCEPT
	{
		for (int64 i{ static_cast<int64>(_InputHistory.Size() - 1) }; i > 0; --i)
		{
			_InputHistory[i] = _InputHistory[i - 1];
		}

		_InputHistory[0] = input_sample;

		for (uint32 sample_index{ 0 }; sample_index < FACTOR; ++sample_index)
		{
			_Samples[sample_index] = SIMD::DotProduct(_TapWeights[sample_index].Data(), _InputHistory.Data(), TAPS_PER_PHASE);
		}
	}

	/*
	*	Returns the samples.
	*/
	FORCE_INLINE NO_DISCARD ArrayProxy<float32> GetSamples() NOEXCEPT
	{
		return ArrayProxy<float32>(&_Samples[0], FACTOR);
	}

	/*
	*	Downsamples.
	*/
	FORCE_INLINE NO_DISCARD float32 DownSample() NOEXCEPT
	{
		for (uint32 sample_index{ 0 }; sample_index < FACTOR; ++sample_index)
		{
			_PhaseBuffers[sample_index][_PhaseIndices[sample_index]] = _Samples[sample_index];
			_PhaseIndices[sample_index] = (_PhaseIndices[sample_index] + 1) % TAPS_PER_PHASE;
		}

		float32	sample{ 0.0f };

		for (uint32 sample_index{ 0 }; sample_index < FACTOR; ++sample_index)
		{
			int32 phase_index{ static_cast<int32>(_PhaseIndices[sample_index]) };

			for (uint32 tap_index{ 0 }; tap_index < TAPS_PER_PHASE; ++tap_index)
			{
				phase_index = (phase_index - 1);
				phase_index += TAPS_PER_PHASE * static_cast<int32>(phase_index < 0);

				sample += _TapWeights[sample_index][tap_index] * _PhaseBuffers[sample_index][phase_index];
			}
		}

		return sample;
	}

private:

	//Constants.
	constexpr static uint32 TAPS_PER_PHASE{ NUMBER_OF_TAPS / FACTOR };

	//The samples.
	StaticArray<float32, FACTOR> _Samples;

	//The input history.
	StaticArray<float32, TAPS_PER_PHASE> _InputHistory;

	//The phase buffers.
	StaticArray<StaticArray<float32, TAPS_PER_PHASE>, FACTOR> _PhaseBuffers;

	//The phase indices.
	StaticArray<uint32, FACTOR> _PhaseIndices;

	//The tap weights.
	StaticArray<StaticArray<float32, TAPS_PER_PHASE>, FACTOR> _TapWeights;

	/*
	*	Designs the filters.
	*/
	FORCE_INLINE void DesignFilters() NOEXCEPT
	{
		//Define constants.
		constexpr float32 HALF_FACTOR{ 0.5f / static_cast<float32>(FACTOR) * 0.95f };
		constexpr float32 MIDDLE{ (static_cast<float32>(NUMBER_OF_TAPS) - 1.0f) * 0.5f };
		constexpr float32 TAPS_RECIPROCAL{ 1.0f / static_cast<float32>(NUMBER_OF_TAPS) };
		constexpr auto SINC_FUNCTION
		{
			[](const float32 X)
			{
				if (X == 0.0f) return 1.0f;

				const float32 _X{ BaseMathConstants::PI * X };

				return std::sin(_X) / _X;
			}
		};

		StaticArray<float32, NUMBER_OF_TAPS> tap_weights;
		Memory::Set(tap_weights.Data(), 0, sizeof(float32) * NUMBER_OF_TAPS);

		for (uint32 tap_index{ 0 }; tap_index < NUMBER_OF_TAPS; ++tap_index)
		{
			const float32 X{ tap_index - MIDDLE };
			const float32 T{ 2.0f * HALF_FACTOR * SINC_FUNCTION(2.0f * HALF_FACTOR * X) };
			const float32 W{ 0.42f - 0.5f * std::cos(2.0f * BaseMathConstants::PI * tap_index * TAPS_RECIPROCAL) + 0.08f * std::cos(4.0f * BaseMathConstants::PI * tap_index * TAPS_RECIPROCAL) };
			
			tap_weights[tap_index] = float(T * W);
		}

		float32 S{ 0.0f };

		for (uint32 tap_index{ 0 }; tap_index < NUMBER_OF_TAPS; ++tap_index)
		{
			S += tap_weights[tap_index];
		}

		if (S != 0.0f)
		{
			const float32 S_reciprocal{ 1.0f / S };

			for (uint32 tap_index{ 0 }; tap_index < NUMBER_OF_TAPS; ++tap_index)
			{
				tap_weights[tap_index] *= S_reciprocal;
			}
		}

		for (uint32 sample_index{ 0 }; sample_index < FACTOR; ++sample_index)
		{
			for (uint32 tap_index{ 0 }; tap_index < TAPS_PER_PHASE; ++tap_index)
			{
				const uint32 index{ tap_index * FACTOR + sample_index };

				_TapWeights[sample_index][tap_index] = tap_weights[index];
			}
		}
	}

};