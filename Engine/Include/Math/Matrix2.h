#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>

class Matrix2 final
{

public:

	//The matrix.
	Vector2<float> _Matrix[2];

	/*
	*	Default constructor.
	*/
	constexpr Matrix2() NOEXCEPT;

	/*
	*	Constructor taking all floats as arguments.
	*/
	FORCE_INLINE constexpr Matrix2(const float X1, const float Y1, const float X2, const float Y2) NOEXCEPT
		:
	 _Matrix{ { X1, Y1 }, { X2, Y2 } }
	{

	}

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	 FORCE_INLINE RESTRICTED NO_DISCARD constexpr float* Data() NOEXCEPT { return &(_Matrix[0]._X); }

	/*
	*	Matrix2 by Vector2 multiplcation overload.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float> operator*(const Vector2<float> &vector) const NOEXCEPT
	{
		return Vector2<float>((_Matrix[0]._X * vector._X) + (_Matrix[1]._X * vector._Y), (_Matrix[0]._Y * vector._X) + (_Matrix[1]._Y * vector._Y));
	}

};