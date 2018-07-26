#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

//Forward declarations.
class Matrix4;

class Matrix3 final
{

public:

	//The matrix.
	Vector3 matrix[3];

	/*
	*	Default constructor.
	*/
	Matrix3() NOEXCEPT;

	/*
	*	Constructor taking a Matrix4.
	*/
	Matrix3(const Matrix4 &otherMatrix) NOEXCEPT;

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	RESTRICTED float* Data() NOEXCEPT { return &(matrix[0].X); }

	/*
	*	Matrix3 by vector3 multiplcation overload.
	*/
	Vector3 operator*(const Vector3 &vector) NOEXCEPT;

};