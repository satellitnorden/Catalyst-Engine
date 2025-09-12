#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Namespace for all things audio.
namespace Audio
{
	//The default sample rate - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_SAMPLE_RATE{ 48'000.0f };

	//The default beats per minute - Certain code assumes this value and initialize using it, until told otherwise.
	constexpr float32 DEFAULT_BEATS_PER_MINUTE{ 120.0f };
}
