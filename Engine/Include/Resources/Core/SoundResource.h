#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/Resource.h>

class SoundResource final : public Resource
{

public:

	//The sample rate.
	float32 _SampleRate;

	//The number of channels.
	uint8 _NumberOfChannels;

	//The samples per channel.
	DynamicArray<DynamicArray<int16>> _Samples;

};