#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Plane final
{

public:

	//The position.
	Vector3<float32> _Position;

	//The normal.
	Vector3<float32> _Normal;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Plane() NOEXCEPT
		:
		_Position(0.0f, 0.0f, 0.0f),
		_Normal(1.0f, 1.0f, 0.0f)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Plane(const Vector3<float32> &initial_position, const Vector3<float32> &initial_normal) NOEXCEPT
		:
		_Position(initial_position),
		_Normal(initial_normal)
	{

	}

};