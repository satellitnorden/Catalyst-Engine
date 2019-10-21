#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class SoundResource final
{

public:

	//The sample rate.
	float _SampleRate;

	//The samples.
	DynamicArray<float> _Samples;

};