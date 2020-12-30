#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

class MIDIDevice final
{

public:

	//The handle to the MIDI device. Will be nullptr until the MIDI device has been opened.
	void *RESTRICT _Handle;

	//The index.
	uint32 _Index;

	//The name.
	DynamicString _Name;

};