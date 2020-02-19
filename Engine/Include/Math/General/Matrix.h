#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/General/Quaternion.h>

class Matrix2x2 final
{

public:

	//The matrix.
	Vector2<float> _Matrix[2];

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Matrix2x2() NOEXCEPT
		:
		_Matrix{ { 1.0f, 0.0f }, { 0.0f, 1.0f } }
	{

	}

	/*
	*	Constructor taking all floats as arguments.
	*/
	FORCE_INLINE constexpr Matrix2x2(const float X1, const float Y1, const float X2, const float Y2) NOEXCEPT
		:
		_Matrix{ { X1, Y1 }, { X2, Y2 } }
	{

	}

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD constexpr float* Data() NOEXCEPT
	{
		return &(_Matrix[0]._X);
	}

	/*
	*	Matrix2 by Vector2 multiplcation overload.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float> operator*(const Vector2<float> &vector) const NOEXCEPT
	{
		return Vector2<float>((_Matrix[0]._X * vector._X) + (_Matrix[1]._X * vector._Y), (_Matrix[0]._Y * vector._X) + (_Matrix[1]._Y * vector._Y));
	}

};

class Matrix3x3 final
{

public:

	//The matrix.
	Vector3<float> _Matrix[3];

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Matrix3x3() NOEXCEPT
		:
		_Matrix{ { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
	{

	}

	/*
	*	Constructor taking the three vectors as arguments.
	*/
	FORCE_INLINE constexpr Matrix3x3(const Vector3<float> &first, const Vector3<float> &second, const Vector3<float> &third) NOEXCEPT
		:
		_Matrix{ first, second, third }
	{

	}

	/*
	*	Matrix3 by vector3 multiplcation overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float> operator*(const Vector3<float> &vector) const NOEXCEPT
	{
		Vector3<float> multiplied_vector{ vector };

		multiplied_vector._X = (_Matrix[0]._X * vector._X) + (_Matrix[1]._X * vector._Y) + (_Matrix[2]._X * vector._Z);
		multiplied_vector._Y = (_Matrix[0]._Y * vector._X) + (_Matrix[1]._Y * vector._Y) + (_Matrix[2]._Y * vector._Z);
		multiplied_vector._Z = (_Matrix[0]._Z * vector._X) + (_Matrix[1]._Z * vector._Y) + (_Matrix[2]._Z * vector._Z);

		return multiplied_vector;
	}

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD float *Data() NOEXCEPT
	{
		return &(_Matrix[0]._X);
	}

};

class Matrix4x4 final
{

public:

	//The matrix.
	Vector4<float> _Matrix[4];

	/*
	*	Calculates a look at matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 LookAt(const Vector3<float> &position, const Vector3<float> &direction, const Vector3<float> &up) NOEXCEPT
	{
		Vector3<float> F{ Vector3<float>::Normalize(direction - position) };
		Vector3<float> S{ Vector3<float>::CrossProduct(F, up) };
		Vector3<float> U{ Vector3<float>::CrossProduct(S, F) };

		Matrix4x4 result;

		result._Matrix[0]._X = S._X;
		result._Matrix[1]._X = S._Y;
		result._Matrix[2]._X = S._Z;

		result._Matrix[0]._Y = U._X;
		result._Matrix[1]._Y = U._Y;
		result._Matrix[2]._Y = U._Z;

		result._Matrix[0]._Z = -F._X;
		result._Matrix[1]._Z = -F._Y;
		result._Matrix[2]._Z = -F._Z;

		result._Matrix[3]._X = -Vector3<float>::DotProduct(S, position);
		result._Matrix[3]._Y = -Vector3<float>::DotProduct(U, position);
		result._Matrix[3]._Z = Vector3<float>::DotProduct(F, position);

		return result;
	}

	/*
	*	Given a normal and an up vector, constructs a rotation matrix.
	*/
	FORCE_INLINE static NO_DISCARD Matrix4x4 Orientation(const Vector3<float> &normal, const Vector3<float> & up) NOEXCEPT
	{
		if (normal == up)
		{
			return Matrix4x4();
		}

		const Vector3<float> rotationAxis{ Vector3<float>::CrossProduct(up, normal) };
		const float rotationAngle{ CatalystBaseMath::ArcCosine(Vector3<float>::DotProduct(normal, up)) };

		const float a = rotationAngle;
		const float c = CatalystBaseMath::Cosine(a);
		const float s = CatalystBaseMath::Sine(a);

		Vector3<float> axis{ Vector3<float>::Normalize(rotationAxis) };
		Vector3<float> temp{ (1.0f - c) * axis };

		Matrix4x4 rotate;

		rotate._Matrix[0]._X = c + temp._X * axis._X;
		rotate._Matrix[0]._Y = temp._X * axis._Y + s * axis._Z;
		rotate._Matrix[0]._Z = temp._X * axis._Z - s * axis._Y;

		rotate._Matrix[1]._X = temp._Y * axis._X - s * axis._Z;
		rotate._Matrix[1]._Y = c + temp._Y * axis._Y;
		rotate._Matrix[1]._Z = temp._Y * axis._Z + s * axis._X;

		rotate._Matrix[2]._X = temp._Z * axis._X + s * axis._Y;
		rotate._Matrix[2]._Y = temp._Z * axis._Y - s * axis._X;
		rotate._Matrix[2]._Z = c + temp._Z * axis._Z;

		Matrix4x4 identity;

		Matrix4x4 result;

		result._Matrix[0] = identity._Matrix[0] * rotate._Matrix[0]._X + identity._Matrix[1] * rotate._Matrix[0]._Y + identity._Matrix[2] * rotate._Matrix[0]._Z;
		result._Matrix[1] = identity._Matrix[0] * rotate._Matrix[1]._X + identity._Matrix[1] * rotate._Matrix[1]._Y + identity._Matrix[2] * rotate._Matrix[1]._Z;
		result._Matrix[2] = identity._Matrix[0] * rotate._Matrix[2]._X + identity._Matrix[1] * rotate._Matrix[2]._Y + identity._Matrix[2] * rotate._Matrix[2]._Z;
		result._Matrix[3] = identity._Matrix[3];

		return result;
	}

	/*
	*	Calculates an ortographic projection matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 Ortographic(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) NOEXCEPT
	{
		Matrix4x4 result;

		result._Matrix[0]._X = 2.0f / (right - left);
		result._Matrix[1]._Y = 2.0f / (top - bottom);
		result._Matrix[2]._Z = -1.0f / (farPlane - nearPlane);
		result._Matrix[3]._X = -(right + left) / (right - left);
		result._Matrix[3]._Y = -(top + bottom) / (top - bottom);
		result._Matrix[3]._Z = -nearPlane / (farPlane - nearPlane);

		result._Matrix[1]._Y *= -1.0f;

		return result;
	}

	/*
	*	Calculates a perspective projection matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 Perspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) NOEXCEPT
	{
		Matrix4x4 result{ 0.0f };

		result._Matrix[0]._X = fov;
		result._Matrix[1]._Y = -aspectRatio;
		result._Matrix[2]._Z = farPlane / (nearPlane - farPlane);
		result._Matrix[2]._W = -1.0f;
		result._Matrix[3]._Z = (nearPlane * farPlane) / (nearPlane - farPlane);

		return result;
	}

	/*
	*	Calculates a reverse perspective projection matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 ReversePerspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) NOEXCEPT
	{
		Matrix4x4 result{ 0.0f };

		result._Matrix[0]._X = fov;
		result._Matrix[1]._Y = -aspectRatio;
		result._Matrix[2]._Z = -(farPlane / (nearPlane - farPlane)) - 1;
		result._Matrix[2]._W = -1.0f;
		result._Matrix[3]._Z = -((nearPlane * farPlane) / (nearPlane - farPlane));

		return result;
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Matrix4x4() NOEXCEPT
		:
		_Matrix{ { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE constexpr Matrix4x4(const Matrix4x4 &otherMatrix) NOEXCEPT
		:
		_Matrix{ otherMatrix._Matrix[0], otherMatrix._Matrix[1], otherMatrix._Matrix[2], otherMatrix._Matrix[3] }
	{

	}

	/*
	*	Constructor taking a scalar.
	*/
	FORCE_INLINE constexpr Matrix4x4(const float scalar) NOEXCEPT
		:
		_Matrix{ { scalar, 0.0f, 0.0f, 0.0f }, { 0.0f, scalar, 0.0f, 0.0f }, { 0.0f, 0.0f, scalar, 0.0f }, { 0.0f, 0.0f, 0.0f, scalar } }
	{

	}

	/*
	*	Constructor taking the four vectors as arguments.
	*/
	FORCE_INLINE constexpr Matrix4x4(const Vector4<float> &vector1, const Vector4<float> &vector2, const Vector4<float> &vector3, const Vector4<float> &vector4) NOEXCEPT
		:
		_Matrix{ vector1, vector2, vector3, vector4 }
	{

	}

	/*
	*	Constructor taking position, rotation and scale as arguments.
	*/
	FORCE_INLINE constexpr Matrix4x4(const Vector3<float> &position, const Vector3<float> &rotation, const Vector3<float> &scale) NOEXCEPT
		:
		_Matrix{ { scale._X, 0.0f, 0.0f, 0.0f },
				 { 0.0f, scale._Y, 0.0f, 0.0f },
				 { 0.0f, 0.0f, scale._Z, 0.0f },
				 { position._X, position._Y, position._Z, 1.0f } }
	{
		//Rotate the matrix.
		Rotate(rotation);
	}

	/*
	*	Constructor taking position and a rotation quaternion as arguments.
	*/
	FORCE_INLINE constexpr Matrix4x4(const Vector3<float> &position, const Quaternion &rotation) NOEXCEPT
		:
		_Matrix{ { 1.0f - 2.0f * (rotation._Y * rotation._Y) - 2.0f * (rotation._Z * rotation._Z), 2.0f * rotation._X * rotation._Y + 2.0f * rotation._W * rotation._Z, 2.0f * rotation._X * rotation._Z - 2.0f * rotation._W * rotation._Y, 0.0f },
				 { 2.0f * rotation._X * rotation._Y - 2.0f * rotation._W * rotation._Z, 1.0f - 2.0f * (rotation._X * rotation._X) - 2.0f * (rotation._Z * rotation._Z), 2.0f * rotation._Y * rotation._Z - 2.0f * rotation._W * rotation._X, 0.0f },
				 { 2.0f * rotation._X * rotation._Z + 2.0f * rotation._W * rotation._Y, 2.0f * rotation._Y * rotation._Z + 2.0f * rotation._W * rotation._X, 1.0f - 2.0f * (rotation._X * rotation._X) - 2.0f * (rotation._Y * rotation._Y), 0.0f },
				 { position._X, position._Y, position._Z, 1.0f } }
	{

	}

	/*
	*	Equality operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool operator==(const Matrix4x4 &other) const NOEXCEPT
	{
		return	_Matrix[0] == other._Matrix[0]
				&& _Matrix[1] == other._Matrix[1]
				&& _Matrix[2] == other._Matrix[2]
				&& _Matrix[3] == other._Matrix[3];
	}

	/*
	*	Inequality operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool operator!=(const Matrix4x4 &other) const NOEXCEPT
	{
		return !(operator==(other));
	}

	/*
	*	Matrix4 by Matrix4 multiplication overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Matrix4x4 operator*(const Matrix4x4 &otherMatrix) const NOEXCEPT
	{
		Matrix4x4 multipliedMatrix{ otherMatrix };

		multipliedMatrix._Matrix[0]._X = (_Matrix[0]._X * otherMatrix._Matrix[0]._X) + (_Matrix[1]._X * otherMatrix._Matrix[0]._Y) + (_Matrix[2]._X * otherMatrix._Matrix[0]._Z) + (_Matrix[3]._X * otherMatrix._Matrix[0]._W);
		multipliedMatrix._Matrix[1]._X = (_Matrix[0]._X * otherMatrix._Matrix[1]._X) + (_Matrix[1]._X * otherMatrix._Matrix[1]._Y) + (_Matrix[2]._X * otherMatrix._Matrix[1]._Z) + (_Matrix[3]._X * otherMatrix._Matrix[1]._W);
		multipliedMatrix._Matrix[2]._X = (_Matrix[0]._X * otherMatrix._Matrix[2]._X) + (_Matrix[1]._X * otherMatrix._Matrix[2]._Y) + (_Matrix[2]._X * otherMatrix._Matrix[2]._Z) + (_Matrix[3]._X * otherMatrix._Matrix[2]._W);
		multipliedMatrix._Matrix[3]._X = (_Matrix[0]._X * otherMatrix._Matrix[3]._X) + (_Matrix[1]._X * otherMatrix._Matrix[3]._Y) + (_Matrix[2]._X * otherMatrix._Matrix[3]._Z) + (_Matrix[3]._X * otherMatrix._Matrix[3]._W);

		multipliedMatrix._Matrix[0]._Y = (_Matrix[0]._Y * otherMatrix._Matrix[0]._X) + (_Matrix[1]._Y * otherMatrix._Matrix[0]._Y) + (_Matrix[2]._Y * otherMatrix._Matrix[0]._Z) + (_Matrix[3]._Y * otherMatrix._Matrix[0]._W);
		multipliedMatrix._Matrix[1]._Y = (_Matrix[0]._Y * otherMatrix._Matrix[1]._X) + (_Matrix[1]._Y * otherMatrix._Matrix[1]._Y) + (_Matrix[2]._Y * otherMatrix._Matrix[1]._Z) + (_Matrix[3]._Y * otherMatrix._Matrix[1]._W);
		multipliedMatrix._Matrix[2]._Y = (_Matrix[0]._Y * otherMatrix._Matrix[2]._X) + (_Matrix[1]._Y * otherMatrix._Matrix[2]._Y) + (_Matrix[2]._Y * otherMatrix._Matrix[2]._Z) + (_Matrix[3]._Y * otherMatrix._Matrix[2]._W);
		multipliedMatrix._Matrix[3]._Y = (_Matrix[0]._Y * otherMatrix._Matrix[3]._X) + (_Matrix[1]._Y * otherMatrix._Matrix[3]._Y) + (_Matrix[2]._Y * otherMatrix._Matrix[3]._Z) + (_Matrix[3]._Y * otherMatrix._Matrix[3]._W);

		multipliedMatrix._Matrix[0]._Z = (_Matrix[0]._Z * otherMatrix._Matrix[0]._X) + (_Matrix[1]._Z * otherMatrix._Matrix[0]._Y) + (_Matrix[2]._Z * otherMatrix._Matrix[0]._Z) + (_Matrix[3]._Z * otherMatrix._Matrix[0]._W);
		multipliedMatrix._Matrix[1]._Z = (_Matrix[0]._Z * otherMatrix._Matrix[1]._X) + (_Matrix[1]._Z * otherMatrix._Matrix[1]._Y) + (_Matrix[2]._Z * otherMatrix._Matrix[1]._Z) + (_Matrix[3]._Z * otherMatrix._Matrix[1]._W);
		multipliedMatrix._Matrix[2]._Z = (_Matrix[0]._Z * otherMatrix._Matrix[2]._X) + (_Matrix[1]._Z * otherMatrix._Matrix[2]._Y) + (_Matrix[2]._Z * otherMatrix._Matrix[2]._Z) + (_Matrix[3]._Z * otherMatrix._Matrix[2]._W);
		multipliedMatrix._Matrix[3]._Z = (_Matrix[0]._Z * otherMatrix._Matrix[3]._X) + (_Matrix[1]._Z * otherMatrix._Matrix[3]._Y) + (_Matrix[2]._Z * otherMatrix._Matrix[3]._Z) + (_Matrix[3]._Z * otherMatrix._Matrix[3]._W);

		multipliedMatrix._Matrix[0]._W = (_Matrix[0]._W * otherMatrix._Matrix[0]._X) + (_Matrix[1]._W * otherMatrix._Matrix[0]._Y) + (_Matrix[2]._W * otherMatrix._Matrix[0]._Z) + (_Matrix[3]._W * otherMatrix._Matrix[0]._W);
		multipliedMatrix._Matrix[1]._W = (_Matrix[0]._W * otherMatrix._Matrix[1]._X) + (_Matrix[1]._W * otherMatrix._Matrix[1]._Y) + (_Matrix[2]._W * otherMatrix._Matrix[1]._Z) + (_Matrix[3]._W * otherMatrix._Matrix[1]._W);
		multipliedMatrix._Matrix[2]._W = (_Matrix[0]._W * otherMatrix._Matrix[2]._X) + (_Matrix[1]._W * otherMatrix._Matrix[2]._Y) + (_Matrix[2]._W * otherMatrix._Matrix[2]._Z) + (_Matrix[3]._W * otherMatrix._Matrix[2]._W);
		multipliedMatrix._Matrix[3]._W = (_Matrix[0]._W * otherMatrix._Matrix[3]._X) + (_Matrix[1]._W * otherMatrix._Matrix[3]._Y) + (_Matrix[2]._W * otherMatrix._Matrix[3]._Z) + (_Matrix[3]._W * otherMatrix._Matrix[3]._W);

		return multipliedMatrix;
	}

	/*
	*	Matrix4 by scalar multiplication assignment overload.
	*/
	FORCE_INLINE constexpr void operator*=(const float scalar) NOEXCEPT
	{
		_Matrix[0]._X *= scalar;
		_Matrix[0]._Y *= scalar;
		_Matrix[0]._Z *= scalar;
		_Matrix[0]._W *= scalar;

		_Matrix[1]._X *= scalar;
		_Matrix[1]._Y *= scalar;
		_Matrix[1]._Z *= scalar;
		_Matrix[1]._W *= scalar;

		_Matrix[2]._X *= scalar;
		_Matrix[2]._Y *= scalar;
		_Matrix[2]._Z *= scalar;
		_Matrix[2]._W *= scalar;

		_Matrix[3]._X *= scalar;
		_Matrix[3]._Y *= scalar;
		_Matrix[3]._Z *= scalar;
		_Matrix[3]._W *= scalar;

		Verify();
	}

	/*
	*	Matrix4 by Vector4 multiplication overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4<float> operator*(const Vector4<float> &vector) const NOEXCEPT
	{
		return Vector4<float>(	(_Matrix[0]._X * vector._X) + (_Matrix[1]._X * vector._Y) + (_Matrix[2]._X * vector._Z) + (_Matrix[3]._X * vector._W),
								(_Matrix[0]._Y * vector._X) + (_Matrix[1]._Y * vector._Y) + (_Matrix[2]._Y * vector._Z) + (_Matrix[3]._Y * vector._W),
								(_Matrix[0]._Z * vector._X) + (_Matrix[1]._Z * vector._Y) + (_Matrix[2]._Z * vector._Z) + (_Matrix[3]._Z * vector._W),
								(_Matrix[0]._W * vector._X) + (_Matrix[1]._W * vector._Y) + (_Matrix[2]._W * vector._Z) + (_Matrix[3]._W * vector._W));
	}

	/*
	*	Returns whether or not this matrix is valid.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsValid() const NOEXCEPT
	{
		//If any of the values in this matrix is NaN, it is considered invalid.
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			for (uint8 j{ 0 }; j < 4; ++j)
			{
				if (CatalystBaseMath::IsNaN(_Matrix[i][j]))
				{
					return false;
				}
			}
		}

		//The matrix passed all tests.
		return true;
	}

	/*
	*	Returns a data to the pointer of this matrix.
	*/
	RESTRICTED constexpr float* Data() NOEXCEPT { return &(_Matrix[0]._X); }

	/*
	*	Returns the translation.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float> GetTranslation() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[3]._X, _Matrix[3]._Y, _Matrix[3]._Z);
	}

	/*
	*	Returns the scale.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float> GetScale() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[0]._X, _Matrix[1]._Y, _Matrix[2]._Z);
	}

	/*
	*	Returns the right.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float> GetRight() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[0]._X, _Matrix[0]._Y, _Matrix[0]._Z);
	}

	/*
	*	Returns the up.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float> GetUp() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[1]._X, _Matrix[1]._Y, _Matrix[1]._Z);
	}

	/*
	*	Returns the forward.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float> GetForward() const NOEXCEPT
	{
		return Vector3<float>(_Matrix[2]._X, _Matrix[2]._Y, _Matrix[2]._Z);
	}

	/*
	*	Sets the translation.
	*/
	FORCE_INLINE constexpr void SetTranslation(const Vector3<float> &newTranslation) NOEXCEPT
	{
		_Matrix[3]._X = newTranslation._X;
		_Matrix[3]._Y = newTranslation._Y;
		_Matrix[3]._Z = newTranslation._Z;

		Verify();
	}

	/*
	*	Sets the scale.
	*/
	FORCE_INLINE constexpr void SetScale(const Vector3<float> &newScale) NOEXCEPT
	{
		_Matrix[0]._X = newScale._X;
		_Matrix[1]._Y = newScale._Y;
		_Matrix[2]._Z = newScale._Z;

		Verify();
	}

	/*
	*	Inverses this matrix.
	*/
	FORCE_INLINE constexpr void Inverse() NOEXCEPT
	{
		float Coef00 = _Matrix[2]._Z * _Matrix[3]._W - _Matrix[3]._Z * _Matrix[2]._W;
		float Coef02 = _Matrix[1]._Z * _Matrix[3]._W - _Matrix[3]._Z * _Matrix[1]._W;
		float Coef03 = _Matrix[1]._Z * _Matrix[2]._W - _Matrix[2]._Z * _Matrix[1]._W;

		float Coef04 = _Matrix[2]._Y * _Matrix[3]._W - _Matrix[3]._Y * _Matrix[2]._W;
		float Coef06 = _Matrix[1]._Y * _Matrix[3]._W - _Matrix[3]._Y * _Matrix[1]._W;
		float Coef07 = _Matrix[1]._Y * _Matrix[2]._W - _Matrix[2]._Y * _Matrix[1]._W;

		float Coef08 = _Matrix[2]._Y * _Matrix[3]._Z - _Matrix[3]._Y * _Matrix[2]._Z;
		float Coef10 = _Matrix[1]._Y * _Matrix[3]._Z - _Matrix[3]._Y * _Matrix[1]._Z;
		float Coef11 = _Matrix[1]._Y * _Matrix[2]._Z - _Matrix[2]._Y * _Matrix[1]._Z;

		float Coef12 = _Matrix[2]._X * _Matrix[3]._W - _Matrix[3]._X * _Matrix[2]._W;
		float Coef14 = _Matrix[1]._X * _Matrix[3]._W - _Matrix[3]._X * _Matrix[1]._W;
		float Coef15 = _Matrix[1]._X * _Matrix[2]._W - _Matrix[2]._X * _Matrix[1]._W;

		float Coef16 = _Matrix[2]._X * _Matrix[3]._Z - _Matrix[3]._X * _Matrix[2]._Z;
		float Coef18 = _Matrix[1]._X * _Matrix[3]._Z - _Matrix[3]._X * _Matrix[1]._Z;
		float Coef19 = _Matrix[1]._X * _Matrix[2]._Z - _Matrix[2]._X * _Matrix[1]._Z;

		float Coef20 = _Matrix[2]._X * _Matrix[3]._Y - _Matrix[3]._X * _Matrix[2]._Y;
		float Coef22 = _Matrix[1]._X * _Matrix[3]._Y - _Matrix[3]._X * _Matrix[1]._Y;
		float Coef23 = _Matrix[1]._X * _Matrix[2]._Y - _Matrix[2]._X * _Matrix[1]._Y;

		Vector4<float> Fac0(Coef00, Coef00, Coef02, Coef03);
		Vector4<float> Fac1(Coef04, Coef04, Coef06, Coef07);
		Vector4<float> Fac2(Coef08, Coef08, Coef10, Coef11);
		Vector4<float> Fac3(Coef12, Coef12, Coef14, Coef15);
		Vector4<float> Fac4(Coef16, Coef16, Coef18, Coef19);
		Vector4<float> Fac5(Coef20, Coef20, Coef22, Coef23);

		Vector4<float> Vec0(_Matrix[1]._X, _Matrix[0]._X, _Matrix[0]._X, _Matrix[0]._X);
		Vector4<float> Vec1(_Matrix[1]._Y, _Matrix[0]._Y, _Matrix[0]._Y, _Matrix[0]._Y);
		Vector4<float> Vec2(_Matrix[1]._Z, _Matrix[0]._Z, _Matrix[0]._Z, _Matrix[0]._Z);
		Vector4<float> Vec3(_Matrix[1]._W, _Matrix[0]._W, _Matrix[0]._W, _Matrix[0]._W);

		Vector4<float> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		Vector4<float> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		Vector4<float> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		Vector4<float> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		Vector4<float> SignA(+1, -1, +1, -1);
		Vector4<float> SignB(-1, +1, -1, +1);
		Matrix4x4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

		Vector4<float> Row0(Inverse._Matrix[0]._X, Inverse._Matrix[1]._X, Inverse._Matrix[2]._X, Inverse._Matrix[3]._X);

		Vector4<float> Dot0(_Matrix[0] * Row0);
		float Dot1 = (Dot0._X + Dot0._Y) + (Dot0._Z + Dot0._W);

		float OneOverDeterminant = 1.0f / Dot1;

		Inverse *= OneOverDeterminant;

		_Matrix[0] = Inverse._Matrix[0];
		_Matrix[1] = Inverse._Matrix[1];
		_Matrix[2] = Inverse._Matrix[2];
		_Matrix[3] = Inverse._Matrix[3];

		Verify();
	}

	/*
	*	Rotates this matrix.
	*/
	FORCE_INLINE constexpr void Rotate(const Vector3<float> &rotation) NOEXCEPT
	{
		//Create a rotation matrix for the X axis.
		if (rotation._X != 0.0f)
		{
			const float xSine = CatalystBaseMath::Sine(rotation._X);
			const float xCosine = CatalystBaseMath::Cosine(rotation._X);

			const Matrix4x4 xRotationMatrix{ Vector4<float>(1.0f, 0.0f, 0.0f, 0.0f), Vector4<float>(0.0f, xCosine, xSine, 0.0f), Vector4<float>(0.0f, -xSine, xCosine, 0.0f), Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f) };

			*this = *this * xRotationMatrix;
		}

		//Create a rotation matrix for the Y axis.
		if (rotation._Y != 0.0f)
		{
			const float ySine = CatalystBaseMath::Sine(rotation._Y);
			const float yCosine = CatalystBaseMath::Cosine(rotation._Y);

			const Matrix4x4 yRotationMatrix{ Vector4<float>(yCosine, 0.0f, -ySine, 0.0f), Vector4<float>(0.0f, 1.0f, 0.0f, 0.0f), Vector4<float>(ySine, 0.0f, yCosine, 0.0f), Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f) };

			*this = *this * yRotationMatrix;
		}

		//Create a rotation matrix for the Z axis.
		if (rotation._Z != 0.0f)
		{
			const float zSine = CatalystBaseMath::Sine(rotation._Z);
			const float zCosine = CatalystBaseMath::Cosine(rotation._Z);

			const Matrix4x4 zRotationMatrix{ Vector4<float>(zCosine, zSine, 0.0f, 0.0f), Vector4<float>(-zSine, zCosine, 0.0f, 0.0f), Vector4<float>(0.0f, 0.0f, 1.0f, 0.0f), Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f) };

			*this = *this * zRotationMatrix;
		}

		Verify();
	}

	/*
	*	Transposes this matrix.
	*/
	FORCE_INLINE constexpr void Transpose() NOEXCEPT
	{
		Vector4<float> transposedMatrix[4];

		transposedMatrix[0]._X = _Matrix[0]._X;
		transposedMatrix[1]._X = _Matrix[0]._Y;
		transposedMatrix[2]._X = _Matrix[0]._Z;
		transposedMatrix[3]._X = _Matrix[0]._W;

		transposedMatrix[0]._Y = _Matrix[1]._X;
		transposedMatrix[1]._Y = _Matrix[1]._Y;
		transposedMatrix[2]._Y = _Matrix[1]._Z;
		transposedMatrix[3]._Y = _Matrix[1]._W;

		transposedMatrix[0]._Z = _Matrix[2]._X;
		transposedMatrix[1]._Z = _Matrix[2]._Y;
		transposedMatrix[2]._Z = _Matrix[2]._Z;
		transposedMatrix[3]._Z = _Matrix[2]._W;

		transposedMatrix[0]._W = _Matrix[3]._X;
		transposedMatrix[1]._W = _Matrix[3]._Y;
		transposedMatrix[2]._W = _Matrix[3]._Z;
		transposedMatrix[3]._W = _Matrix[3]._W;

		_Matrix[0] = transposedMatrix[0];
		_Matrix[1] = transposedMatrix[1];
		_Matrix[2] = transposedMatrix[2];
		_Matrix[3] = transposedMatrix[3];

		Verify();
	}

	/*
	*	Verrifies this matrix.
	*/
	FORCE_INLINE constexpr void Verify() const NOEXCEPT
	{
		if (!IsValid())
		{
			CRASH();
		}
	}

};

//Matrix constants.
namespace MatrixConstants
{
	constexpr Matrix4x4 IDENTITY{  };
}