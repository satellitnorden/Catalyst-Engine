#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Biquad.h>

/*
*	Small filter class that processes a signal to make it closer to human hearing.
*	Good to do if you're trying to measure loudness.
*/
class PerceptualLoudnessFilter final
{

public:

	/*
	*	Initializes this perceptual loudness filter.
	*/
	FORCE_INLINE void Initialize(const float32 sample_rate) NOEXCEPT
	{
		//Initialize the high pass.
		_HighPass.InitializeHighPass(60.0f, 1.0f, sample_rate);

		//Initialize the high shelf.
		_HighShelf.InitializeHighShelf(4'000.0f, Audio::DecibelsToGain(4.0f), sample_rate);
	}

	/*
	*	Processes a single sample.
	*/
	FORCE_INLINE NO_DISCARD float32 Process(const float32 input) NOEXCEPT
	{
		float32 output{ input };

		_HighPass.Process(&output, &output, 1);
		_HighShelf.Process(&output, &output, 1);

		return output;
	}

private:

	//The high pass.
	Biquad _HighPass;

	//The high shelf.
	Biquad _HighShelf;

};