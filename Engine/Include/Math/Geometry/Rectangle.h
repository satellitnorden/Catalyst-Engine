#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class Rectangle final
{

public:

	//The position.
	Vector2<float32> _Position;

	//The size.
	Vector2<float32> _Size;

	//The rotation.
	float32 _Rotation;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Rectangle() NOEXCEPT
		:
		_Position(0.0f, 0.0f),
		_Size(1.0f, 1.0f),
		_Rotation(0.0f)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Rectangle(const Vector2<float32> &initial_position, const Vector2<float32> &initial_size, const float32 initial_rotation) NOEXCEPT
		:
		_Position(initial_position),
		_Size(initial_size),
		_Rotation(initial_rotation)
	{

	}

};