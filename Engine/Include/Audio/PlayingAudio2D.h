#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/AudioStreamPlayer.h>

class PlayingAudio2D final
{

public:

	//The identifier.
	Audio::Identifier _Identifier;

	//The player.
	AudioStreamPlayer _Player;

};