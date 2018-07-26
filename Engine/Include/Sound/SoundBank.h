#pragma once

//Core.
#include <Core/EngineCore.h>

//Sound.
#include <Sound/SoundCore.h>

class SoundBank final
{

public:

	//The FMOD bank.
	FMOD::Studio::Bank *RESTRICT bank;

};