#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Ray final
{

public:

	//The origin.
	Vector3<float> _Origin;

	//The direction.
	Vector3<float> _Direction;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Ray() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Ray(const Vector3<float> &inital_origin, const Vector3<float> &initial_direction) NOEXCEPT
		:
		_Origin(inital_origin),
		_Direction(initial_direction)
	{
		
	}

};