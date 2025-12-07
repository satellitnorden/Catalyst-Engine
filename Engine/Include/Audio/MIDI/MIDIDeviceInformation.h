#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

class MIDIDeviceInformation final
{

public:

	//The name.
	DynamicString _Name;

	//The index.
	uint32 _Index;

};