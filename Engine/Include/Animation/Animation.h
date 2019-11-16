#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/AnimationKeyframe.h>

class Animation final
{

public:

	//The duration of this animation, denoted in seconds.
	float _Duration;

	//Container for all the keyframes for all bones.
	DynamicArray<AnimationKeyframe> _Keyframes;

};