#pragma once

//Core.
#include <Core/EngineCore.h>

//Sound.
#include <Sound/SoundCore.h>

class Sound2DComponent final
{

public:

#if defined(CATALYST_WINDOWS)
	//The event instance.
	FMOD::Studio::EventInstance *RESTRICT eventInstance;
#endif

};