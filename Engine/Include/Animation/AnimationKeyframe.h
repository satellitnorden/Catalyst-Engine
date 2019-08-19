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

	//The bone name
	HashString _BoneName;

	//The bone transform.
	BoneTransform _BoneTransform;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr AnimationKeyframe(const float initial_timestamp, const HashString initial_bone_name, const BoneTransform &initial_bone_transform) NOEXCEPT
		:
		_Timestamp(initial_timestamp),
		_BoneName(initial_bone_name),
		_BoneTransform(initial_bone_transform)
	{

	}

};