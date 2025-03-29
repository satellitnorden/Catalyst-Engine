#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Quaternion.h>
#include <Math/General/Vector.h>

class BoneTransform final
{

public:

	//The translation.
	Vector3<float32> _Translation;

	//The rotation.
	Quaternion _Rotation;

	//The scale.
	Vector3<float32> _Scale;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE BoneTransform(const Vector3<float32> &initial_translation, const Quaternion &initial_rotation, const Vector3<float32> &initial_scale) NOEXCEPT
		:
		_Translation(initial_translation),
		_Rotation(initial_rotation),
		_Scale(initial_scale)
	{

	}

};