#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Capsule final
{

public:

	//The lower position.
	Vector3<float> _Lower;

	//The upper position.
	Vector3<float> _Upper;

	//The radius.
	float _Radius;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Capsule() NOEXCEPT
		:
		_Lower(0.0f, 0.0f, 0.0f),
		_Upper(0.0f, 0.0f, 0.0f),
		_Radius(0.0f)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Capsule(const Vector3<float> &initial_lower, const Vector3<float> &initial_upper, const float initial_radius) NOEXCEPT
		:
		_Lower(initial_lower),
		_Upper(initial_upper),
		_Radius(initial_radius)
	{

	}

};