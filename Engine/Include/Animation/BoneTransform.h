#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Quaternion.h>
#include <Math/General/Vector.h>

class BoneTransform final
{

public:

	//The rotation.
	Quaternion _Rotation;

	//The position.
	Vector3<float> _Position;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE BoneTransform(const Quaternion &initial_rotation, const Vector3<float> &initial_position) NOEXCEPT
		:
		_Rotation(initial_rotation),
		_Position(initial_position)
	{

	}

};