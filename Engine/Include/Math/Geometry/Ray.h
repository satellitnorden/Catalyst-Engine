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
	*	Constructor taking all values as arguments.
	*/
	Ray(const Vector3<float> &initalOrigin, const Vector3<float> &initialDirection) NOEXCEPT
		:
		_Origin(initalOrigin),
		_Direction(initialDirection)
	{

	}

	/*
	*	Returns whether or not a ray is valid.
	*/
	static NO_DISCARD bool IsValid(const Ray &ray) NOEXCEPT
	{
		return ray._Direction.IsNormalized();
	}

};