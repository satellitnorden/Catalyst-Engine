#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class SoundResource final
{

public:

	//The sample rate.
	float _SampleRate;

	//The number of channels.
	uint8 _NumberOfChannels;

	//The samples.
	DynamicArray<float> _Samples;

};