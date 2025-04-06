#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Animation.
#include <Animation/AnimationKeyframe.h>

class AnimationChannel final
{

public:

	//The bone identifier.
	HashString _BoneIdentifier;

	//The keyframes.
	DynamicArray<AnimationKeyframe> _Keyframes;

};