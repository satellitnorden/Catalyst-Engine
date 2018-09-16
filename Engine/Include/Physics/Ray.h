#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class Ray final
{

public:

	//The origin.
	Vector3 _Origin;

	//The direction.
	Vector3 _Direction;

	//The distance.
	float _Distance;

	/*
	*	Returns whether or not a ray is valid.
	*/
	static NO_DISCARD bool IsValid(const Ray &ray) NOEXCEPT
	{
		return	ray._Direction.IsNormalized()
				&& ray._Distance != 0.0f;
	}

};