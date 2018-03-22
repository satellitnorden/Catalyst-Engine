#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

//Sound.
#include <Sound/SoundCore.h>

class Sound3DComponent final
{

public:

	//The event instance.
	FMOD::Studio::EventInstance *RESTRICT eventInstance;

	//The position.
	Vector3 position;

};