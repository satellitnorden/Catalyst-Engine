#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector3.h>

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
	constexpr Plane(const Vector3<float> &initialPosition, const Vector3<float> &initialNormal) NOEXCEPT
		:
		_Position(initialPosition),
		_Normal(initialNormal)
	{

	}

};