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

	//The maximum hit distance.
	float _MaximumHitDistance;

	/*
	*	Constructor taking all values as arguments.
	*/
	Ray(const Vector3<float> &initalOrigin, const Vector3<float> &initialDirection, const float initialMaximumHitDistance) NOEXCEPT
		:
		_Origin(initalOrigin),
		_Direction(initialDirection),
		_MaximumHitDistance(initialMaximumHitDistance)
	{
		
	}

};