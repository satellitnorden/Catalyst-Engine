#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/AudioAsset.h>

class PlayAudio2DRequest final
{

public:

	//The audio track identifier. Will default to the master track if not set.
	Audio::Identifier _AudioTrackIdentifier{ Audio::INVALID_IDENTIFIER };

	//The asset.
	AssetPointer<AudioAsset> _Asset;

	//The gain.
	float32 _Gain{ 1.0f };

	//The pan, where -1.0f is 100% to the left, and +1.0f is 100% to the right.
	float32 _Pan{ 0.0f };

	//The start time.
	float32 _StartTime{ 0.0f };

	//The playback rate.
	float32 _PlaybackRate{ 1.0f };

};