#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Circle final
{

public:

	//The position.
	Vector2<float32> _Position;

	//The radius.
	float32 _Radius;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Circle() NOEXCEPT
		:
		_Position(0.0f, 0.0f),
		_Radius(1.0f)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Circle(const Vector2<float32> &initial_position, const float32 initial_radius) NOEXCEPT
		:
		_Position(initial_position),
		_Radius(initial_radius)
	{

	}

};