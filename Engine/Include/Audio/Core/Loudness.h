#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Core/Audio.h>

/*
*	Class dealing with loudness.
*	Use 'Measure()' to measure the perceptual loudness of a signal.
*/
class Loudness final
{

public:

	//Constants.
	constexpr static float32 GUITAR_PLUGIN_TARGET{ -21.0f }; //Normal electric guitar playing should sit around -18 to -24dB, this is right in the middle of that. (:

	/*
	*	Measures the loudness of the given signal.
	*	Returns the loudness in dB.
	*/
	FORCE_INLINE static NO_DISCARD float32 Measure
	(
		const float32 sample_rate,
		const float32 *const RESTRICT samples,
		const uint32 number_of_samples
	) NOEXCEPT
	{
        float64 sum{ 0.0 };

        for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
        {
			float32 sample{ samples[sample_index] };

            sum += static_cast<float64>(sample) * static_cast<float64>(sample);
        }

        sum /= static_cast<float64>(number_of_samples);
		sum = std::sqrt(sum);

        return Audio::GainToDecibels(static_cast<float32>(sum));
	}

};