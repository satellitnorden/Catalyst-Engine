#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Plane final
{

public:

	//The position.
	Vector3<float> _Position;

	//The normal.
	Vector3<float> _Normal;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Plane(const Vector3<float> &initial_position, const Vector3<float> &initial_normal) NOEXCEPT
		:
		_Position(initial_position),
		_Normal(initial_normal)
	{

	}

};