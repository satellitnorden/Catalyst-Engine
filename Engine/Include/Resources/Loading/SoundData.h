#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class SoundData final
{

public:

	//The sample rate.
	float32 _SampleRate;

	//The number of channels.
	uint8 _NumberOfChannels;

	//The samples per channel.
	DynamicArray<DynamicArray<int16>> _Samples;

};