#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Audio.
#include <Audio/Core/Audio.h>

class AudioDeviceInformation final
{

public:

	//The backend for this audio device.
	Audio::Backend _Backend{ Audio::Backend::NONE };

	//The name.
	DynamicString _Name;

	//The identifier.
	uint32 _Identifier;

	//Denotes whether or not this is the default audio device.
	bool _IsDefault;

};