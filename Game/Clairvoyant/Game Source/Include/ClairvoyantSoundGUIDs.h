#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Systems.
#include <Systems/SoundSystem.h>

namespace ClairvoyantSoundGUIDs
{
	static const FMOD_GUID KICK{ SoundSystem::ParseGUID("{ca55450a-6885-493e-9cc4-f5ccbba73f06}") };
	static const FMOD_GUID SPLATT{ SoundSystem::ParseGUID("{cffbc53c-d7b5-4466-a6ed-c208e185c656}") };
	static const FMOD_GUID WIND{ SoundSystem::ParseGUID("{ac36f0db-1d51-4c82-9110-97418c3f81d5}") };
}