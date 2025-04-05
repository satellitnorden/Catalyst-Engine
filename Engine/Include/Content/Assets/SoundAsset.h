#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Content.
#include <Content/Core/Asset.h>

class SoundAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The sample rate.
	float32 _SampleRate;

	//The number of channels.
	uint8 _NumberOfChannels;

	//The samples per channel.
	DynamicArray<DynamicArray<int16>> _Samples;

};