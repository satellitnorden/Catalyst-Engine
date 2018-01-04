#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

//Forward declarations.
class Matrix4;

class Matrix3
{

public:

	//The matrix.
	Vector3 matrix[3];

	/*
	*	Default constructor.
	*/
	Matrix3() CATALYST_NOEXCEPT;

	/*
	*	Constructor taking a Matrix4.
	*/
	Matrix3(const Matrix4 &otherMatrix) CATALYST_NOEXCEPT;

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	CATALYST_RESTRICTED float* Data() CATALYST_NOEXCEPT { return &(matrix[0].X); }

	/*
	*	Matrix3 by vector3 multiplcation overload.
	*/
	Vector3 operator*(const Vector3 &vector) CATALYST_NOEXCEPT;

};