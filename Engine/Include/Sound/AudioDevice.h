#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

class AudioDevice final
{

public:

	//The handle to the audio device. Will be nullptr until the audio device has been opened.
	void *RESTRICT _Handle;

	//The index.
	uint32 _Index;

	//The name.
	DynamicString _Name;

	//Denotes whether or not this audio device is the default one.
	bool _IsDefault;

	//The available bit depts.
	DynamicArray<uint8> _AvailableBitDepths;

	//The available samples rates.
	DynamicArray<float32> _AvailableSampleRates;

	//The preferred sample rate.
	float32 _PreferredSampleRate;

};