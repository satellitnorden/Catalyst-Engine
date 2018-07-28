#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

//Sound.
#include <Sound/SoundCore.h>

class Sound3DComponent final
{

public:

#if defined(CATALYST_WINDOWS)
	//The event instance.
	FMOD::Studio::EventInstance *RESTRICT eventInstance;
#endif

	//The position.
	Vector3 position;

};