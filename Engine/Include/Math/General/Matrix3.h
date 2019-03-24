#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Forward declarations.
class Matrix4;

class Matrix3 final
{

public:

	//The matrix.
	Vector3<float> _Matrix[3];

	/*
	*	Default constructor.
	*/
	Matrix3() NOEXCEPT;

	/*
	*	Constructor taking a Matrix4.
	*/
	Matrix3(const Matrix4 &otherMatrix) NOEXCEPT;

	/*
	*	Constructor taking the three vectors as arguments.
	*/
	Matrix3(const Vector3<float> &first, const Vector3<float> &second, const Vector3<float> &third) NOEXCEPT;

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	RESTRICTED float* Data() NOEXCEPT { return &(_Matrix[0]._X); }

	/*
	*	Matrix3 by vector3 multiplcation overload.
	*/
	Vector3<float> operator*(const Vector3<float> &vector) const NOEXCEPT;

};