#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/AudioTrackInformation.h>

class AudioTrack final
{

public:

	//The information.
	AudioTrackInformation _Information;

	//The identifier.
	Audio::Identifier _Identifier;

	//The samples.
	DynamicArray<DynamicArray<float32>> _Samples;

	//The effects.
	DynamicArray<AudioEffect *RESTRICT> _Effects;

};