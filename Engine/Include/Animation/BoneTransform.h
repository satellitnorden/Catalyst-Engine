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
	*	Default constructor.
	*/
	FORCE_INLINE BoneTransform() NOEXCEPT
	{

	}

};