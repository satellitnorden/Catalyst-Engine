#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

//Content.
#include <Content/Core/Asset.h>

class AudioAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The audio stream.
	AudioStream _AudioStream;

};