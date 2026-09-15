#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/AudioTrackInformation.h>
#include <Audio/PlayingAudio2D.h>

class AudioTrack final
{

public:

	//The information.
	AudioTrackInformation _Information;

	//The identifier.
	Audio::Identifier _Identifier;

	//Denotes whether or not this audio track is solo'd.
	bool _Solo;

	//Denotes whether or not this audio track is muted.
	bool _Mute;

	//The playing audio 2D.
	DynamicArray<PlayingAudio2D> _PlayingAudio2D;

	//The samples.
	DynamicArray<DynamicArray<float32>> _Samples;

	//The effects.
	DynamicArray<AudioEffect *RESTRICT> _Effects;

};