#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Sound.
#include <Sound/SoundCore.h>

class SoundBank final
{

public:

	//The FMOD bank.
	FMOD::Studio::Bank *RESTRICT bank;

};