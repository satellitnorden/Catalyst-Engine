#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector4.h>

//Forward declarations.
class Matrix3;
template <typename Type>
class Vector3;

class Matrix4 final
{

public:

	//The matrix.
	Vector4<float> _Matrix[4];

	/*
	*	Calculates a look at matrix.
	*/
	static NO_DISCARD Matrix4 LookAt(const Vector3<float> &position, const Vector3<float> &direction, const Vector3<float> &up) NOEXCEPT;

	/*
	*	Given a normal and an up vector, constructs a rotation matrix.
	*/
	static NO_DISCARD Matrix4 Orientation(const Vector3<float> &normal, const Vector3<float> & up) NOEXCEPT;

	/*
	*	Calculates an ortographic projection matrix.
	*/
	static NO_DISCARD Matrix4 Ortographic(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) NOEXCEPT;

	/*
	*	Calculates a perspective projection matrix.
	*/
	static NO_DISCARD Matrix4 Perspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) NOEXCEPT;

	/*
	*	Calculates a reverse perspective projection matrix.
	*/
	static NO_DISCARD Matrix4 ReversePerspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) NOEXCEPT;

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
	Matrix4(const Vector4<float> &vector1, const Vector4<float> &vector2, const Vector4<float> &vector3, const Vector4<float> &vector4) NOEXCEPT;

	/*
	*	Constructor taking in position, rotation and scale as arguments.
	*/
	Matrix4(const Vector3<float> &position, const Vector3<float> &rotation, const Vector3<float> &scale) NOEXCEPT;

	/*
	*	Equality operator overload.
	*/
	FORCE_INLINE constexpr bool operator==(const Matrix4 &other) NOEXCEPT
	{
		return	_Matrix[0] == other._Matrix[0]
				&& _Matrix[1] == other._Matrix[1]
				&& _Matrix[2] == other._Matrix[2]
				&& _Matrix[3] == other._Matrix[3];
	}

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
	Vector4<float> operator*(const Vector4<float> &vector) const NOEXCEPT;

	/*
	*	Returns whether or not this matrix is valid.
	*/
	NO_DISCARD bool IsValid() const NOEXCEPT;

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	RESTRICTED float* Data() NOEXCEPT { return &(_Matrix[0]._X); }

	/*
	*	Returns the translation.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> GetTranslation() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[3]._X, _Matrix[3]._Y, _Matrix[3]._Z);
	}

	/*
	*	Returns the scale.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> GetScale() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[0]._X, _Matrix[1]._Y, _Matrix[2]._Z);
	}

	/*
	*	Returns the right.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> GetRight() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[0]._X, _Matrix[0]._Y, _Matrix[0]._Z);
	}

	/*
	*	Returns the up.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> GetUp() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[1]._X, _Matrix[1]._Y, _Matrix[1]._Z);
	}

	/*
	*	Returns the forward.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> GetForward() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[1]._X, _Matrix[1]._Y, _Matrix[1]._Z);
	}

	/*
	*	Sets the translation.
	*/
	void SetTranslation(const Vector3<float> &newTranslation) NOEXCEPT;

	/*
	*	Sets the scale.
	*/
	void SetScale(const Vector3<float> &newScale) NOEXCEPT;
	
	/*
	*	Inverses this matrix.
	*/
	void Inverse() NOEXCEPT;

	/*
	*	Rotates this matrix.
	*/
	void Rotate(const Vector3<float> &rotation) NOEXCEPT;

	/*
	*	Transposes this matrix.
	*/
	void Transpose() NOEXCEPT;

};