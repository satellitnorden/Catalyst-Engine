#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Animation.
#include <Animation/BoneTransform.h>

class AnimationKeyframe final
{

public:

	//The timestamp, denoted in seconds.
	float32 _Timestamp;

	//The bone transform.
	BoneTransform _BoneTransform;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AnimationKeyframe() NOEXCEPT
	{

	}

};