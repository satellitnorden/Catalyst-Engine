#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector4.h>

//Forward declarations.
class Matrix3;
class Vector3;

class Matrix4 final
{

public:

	//The matrix.
	Vector4 matrix[4];

	/*
	*	Calculates a look at matrix.
	*/
	static Matrix4 LookAt(const Vector3 &position, const Vector3 &direction, const Vector3 &up) CATALYST_NOEXCEPT;

	/*
	*	Calculates an ortographic projection matrix.
	*/
	static Matrix4 Ortographic(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) CATALYST_NOEXCEPT;

	/*
	*	Calculates a perspective projection matrix.
	*/
	static Matrix4 Perspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) CATALYST_NOEXCEPT;

	/*
	*	Default constructor.
	*/
	Matrix4() CATALYST_NOEXCEPT;

	/*
	*	Copy constructor.
	*/
	Matrix4(const Matrix4 &otherMatrix) CATALYST_NOEXCEPT;

	/*
	*	Constructor taking a scalar.
	*/
	Matrix4(const float scalar) CATALYST_NOEXCEPT;

	/*
	*	Constructor taking a Matrix3.
	*/
	Matrix4(const Matrix3 &otherMatrix) CATALYST_NOEXCEPT;

	/*
	*	Constructor taking the four vectors as arguments.
	*/
	Matrix4(const Vector4 &vector1, const Vector4 &vector2, const Vector4 &vector3, const Vector4 &vector4) CATALYST_NOEXCEPT;

	/*
	*	Constructor taking in position, rotation and scale as arguments.
	*/
	Matrix4(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) CATALYST_NOEXCEPT;

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	CATALYST_RESTRICTED float* Data() CATALYST_NOEXCEPT { return &(matrix[0].X); }

	/*
	*	Returns the translation.
	*/
	Vector3 GetTranslation() const CATALYST_NOEXCEPT;

	/*
	*	Returns the scale.
	*/
	Vector3 GetScale() const CATALYST_NOEXCEPT;

	/*
	*	Sets the translation.
	*/
	void SetTranslation(const Vector3 &newTranslation) CATALYST_NOEXCEPT;

	/*
	*	Sets the scale.
	*/
	void SetScale(const Vector3 & newScale) CATALYST_NOEXCEPT;

	/*
	*	Returns the determinant of this matrix.
	*/
	float GetDeterminant() CATALYST_NOEXCEPT;
	
	/*
	*	Inverses the matrix.
	*/
	void Inverse() CATALYST_NOEXCEPT;

	/*
	*	Transposes the matrix.
	*/
	void Transpose() CATALYST_NOEXCEPT;

	/*
	*	Matrix4 by Matrix4 multiplication overload.
	*/
	Matrix4 operator*(const Matrix4 &otherMatrix) const CATALYST_NOEXCEPT;

	/*
	*	Matrix4 by scalar multiplication assignment overload.
	*/
	void operator*=(const float scalar) CATALYST_NOEXCEPT;

	/*
	*	Matrix4 by Vector4 multiplication overload.
	*/
	Vector4 operator*(const Vector4 &vector) const CATALYST_NOEXCEPT;

};