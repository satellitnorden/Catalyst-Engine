#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>

//Animation.
#include <Animation/AnimationKeyframe.h>

//Resources.
#include <Resources/Core/Resource.h>

class AnimationResource final : public Resource
{

public:

	//The duration of this animation, denoted in seconds.
	float32 _Duration;

	//Container for all the keyframes for all bones.
	Map<HashString, DynamicArray<AnimationKeyframe>> _Keyframes;

};