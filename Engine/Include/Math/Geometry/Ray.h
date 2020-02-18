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

	//The reciprocals.
	Vector3<float> _Reciprocals;

	//The signs.
	Vector3<uint8> _Signs;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Ray() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Ray(const Vector3<float> &initial_origin, const Vector3<float> &initial_direction) NOEXCEPT
	{
		SetOrigin(initial_origin);
		SetDirection(initial_direction);
	}

	/*
	*	Sets the origin.
	*/
	FORCE_INLINE constexpr void SetOrigin(const Vector3<float> &origin) NOEXCEPT
	{
		_Origin = origin;
	}

	/*
	*	Sets the direction.
	*/
	FORCE_INLINE constexpr void SetDirection(const Vector3<float> &direction) NOEXCEPT
	{
		_Direction = direction;
		_Reciprocals = Vector3<float>(1.0f / _Direction._X, 1.0f / _Direction._Y, 1.0f / _Direction._Z);
		_Signs = Vector3<uint8>(_Direction._X >= 0.0f ? 1 : 0, _Direction._Y >= 0.0f ? 1 : 0, _Direction._Z >= 0.0f ? 1 : 0);
	}

};