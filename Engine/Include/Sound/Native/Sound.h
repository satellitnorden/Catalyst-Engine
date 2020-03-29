#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class Sound final
{

public:

	//The sample rate.
	float32 _SampleRate;

	//The number of channels.
	uint8 _NumberOfChannels;

	//The samples per channel.
	DynamicArray<DynamicArray<float32>> _Samples;

};