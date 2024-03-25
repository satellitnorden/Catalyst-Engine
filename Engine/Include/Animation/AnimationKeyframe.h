#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Animation.
#include <Animation/BoneTransform.h>

class AnimationKeyframe final
{

public:

	//The timestamp, denoted in seconds.
	float _Timestamp;

	//The bone transform.
	BoneTransform _BoneTransform;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE AnimationKeyframe(const float initial_timestamp, const BoneTransform &initial_bone_transform) NOEXCEPT
		:
		_Timestamp(initial_timestamp),
		_BoneTransform(initial_bone_transform)
	{

	}

};