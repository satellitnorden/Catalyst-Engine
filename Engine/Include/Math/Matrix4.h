#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector4.h>

//Forward declarations.
class Matrix3;
class Vector3;

class Matrix4 final
{

public:

	//The matrix.
	Vector4 _Matrix[4];

	/*
	*	Calculates a look at matrix.
	*/
	static Matrix4 LookAt(const Vector3 &position, const Vector3 &direction, const Vector3 &up) NOEXCEPT;

	/*
	*	Calculates an ortographic projection matrix.
	*/
	static Matrix4 Ortographic(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) NOEXCEPT;

	/*
	*	Calculates a perspective projection matrix.
	*/
	static Matrix4 Perspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) NOEXCEPT;

	/*
	*	Default constructor.
	*/
	Matrix4() NOEXCEPT;

	/*
	*	Copy constructor.
	*/
	Matrix4(const Matrix4 &otherMatrix) NOEXCEPT;

	/*
	*	Constructor taking a scalar.
	*/
	Matrix4(const float scalar) NOEXCEPT;

	/*
	*	Constructor taking a Matrix3.
	*/
	Matrix4(const Matrix3 &otherMatrix) NOEXCEPT;

	/*
	*	Constructor taking the four vectors as arguments.
	*/
	Matrix4(const Vector4 &vector1, const Vector4 &vector2, const Vector4 &vector3, const Vector4 &vector4) NOEXCEPT;

	/*
	*	Constructor taking in position, rotation and scale as arguments.
	*/
	Matrix4(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) NOEXCEPT;

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	RESTRICTED float* Data() NOEXCEPT { return &(_Matrix[0]._X); }

	/*
	*	Returns the translation.
	*/
	Vector3 GetTranslation() const NOEXCEPT;

	/*
	*	Returns the scale.
	*/
	Vector3 GetScale() const NOEXCEPT;

	/*
	*	Sets the translation.
	*/
	void SetTranslation(const Vector3 &newTranslation) NOEXCEPT;

	/*
	*	Sets the scale.
	*/
	void SetScale(const Vector3 & newScale) NOEXCEPT;

	/*
	*	Returns the determinant of this matrix.
	*/
	float GetDeterminant() NOEXCEPT;
	
	/*
	*	Inverses the matrix.
	*/
	void Inverse() NOEXCEPT;

	/*
	*	Transposes the matrix.
	*/
	void Transpose() NOEXCEPT;

	/*
	*	Matrix4 by Matrix4 multiplication overload.
	*/
	Matrix4 operator*(const Matrix4 &otherMatrix) const NOEXCEPT;

	/*
	*	Matrix4 by scalar multiplication assignment overload.
	*/
	void operator*=(const float scalar) NOEXCEPT;

	/*
	*	Matrix4 by Vector4 multiplication overload.
	*/
	Vector4 operator*(const Vector4 &vector) const NOEXCEPT;

};