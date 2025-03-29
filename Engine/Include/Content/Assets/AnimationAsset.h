#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/AnimationChannel.h>

//Content.
#include <Content/Core/Asset.h>

class AnimationAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The duration, in seconds.
	float32 _Duration;

	//The channels.
	DynamicArray<AnimationChannel> _Channels;

};