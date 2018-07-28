#pragma once

//Core.
#include <Core/EngineCore.h>

//Sound.
#include <Sound/SoundCore.h>

class SoundBank final
{

public:

#if defined(CATALYST_WINDOWS)
	//The FMOD bank.
	FMOD::Studio::Bank *RESTRICT bank;
#endif

};