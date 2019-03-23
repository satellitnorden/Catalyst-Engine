#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector3.h>

class Sphere final
{

public:

	//The position.
	Vector3<float> _Position;

	//The radius.
	float _Radius;

	/*
	*	Constructor taking all values as arguments.
	*/
	constexpr Sphere(const Vector3<float> &initialPosition, const float initialRadius) NOEXCEPT
		:
		_Position(initialPosition),
		_Radius(initialRadius)
	{

	}

};