#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Sound.
#include <Sound/SoundCore.h>

class Sound2DComponent final
{

public:

	//The event instance.
	FMOD::Studio::EventInstance *RESTRICT eventInstance;

};