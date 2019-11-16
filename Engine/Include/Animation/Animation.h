#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>

//Animation.
#include <Animation/AnimationKeyframe.h>

class Animation final
{

public:

	//The duration of this animation, denoted in seconds.
	float _Duration;

	//Container for all the keyframes for all bones.
	Map<HashString, DynamicArray<AnimationKeyframe>> _Keyframes;

};