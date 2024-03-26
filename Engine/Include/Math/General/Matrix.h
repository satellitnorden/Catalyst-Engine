#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/EulerAngles.h>
#include <Math/General/Vector.h>

//Forward declarations.
class Quaternion;

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

	union
	{
		//The matrix.
		Vector4<float32> _Matrix[4];

		struct
		{
			float32 _11;
			float32 _12;
			float32 _13;
			float32 _14;

			float32 _21;
			float32 _22;
			float32 _23;
			float32 _24;

			float32 _31;
			float32 _32;
			float32 _33;
			float32 _34;

			float32 _41;
			float32 _42;
			float32 _43;
			float32 _44;
		};
	};

	/*
	*	Calculates a look at matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 LookAt(const Vector3<float32> &position, const Vector3<float32> &target, const Vector3<float32> &up) NOEXCEPT
	{
		Vector3<float32> F{ Vector3<float32>::Normalize(target - position) };
		Vector3<float32> S{ Vector3<float32>::CrossProduct(F, up) };
		Vector3<float32> U{ Vector3<float32>::CrossProduct(S, F) };

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
	FORCE_INLINE static NO_DISCARD Matrix4x4 Orientation(const Vector3<float32> &normal, const Vector3<float32> &up) NOEXCEPT
	{
		if (normal == up)
		{
			return Matrix4x4();
		}

		const Vector3<float32> rotationAxis{ Vector3<float32>::CrossProduct(up, normal) };
		const float32 rotationAngle{ CatalystBaseMath::ArcCosine(Vector3<float>::DotProduct(normal, up)) };

		const float32 a = rotationAngle;
		const float32 c = CatalystBaseMath::Cosine(a);
		const float32 s = CatalystBaseMath::Sine(a);

		Vector3<float32> axis{ Vector3<float32>::Normalize(rotationAxis) };
		Vector3<float32> temp{ (1.0f - c) * axis };

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
	*	Calculates an orthographic projection matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 Orthographic(const float32 left_plane, const float32 right_plane, const float32 bottom_plane, const float32 top_plane, const float32 near_plane, const float32 far_plane) NOEXCEPT
	{
		return Matrix4x4(	-(2.0f / (right_plane - left_plane)),
							0.0f,
							0.0f,
							0.0f,

							0.0f,
							2.0f / (bottom_plane - top_plane),
							0.0f,
							0.0f,

							0.0f,
							0.0f,
							1.0f / (-far_plane - -near_plane),
							0.0f,

							-(-(right_plane + left_plane) / (right_plane - left_plane)),
							-(bottom_plane + top_plane) / (bottom_plane - top_plane),
							-far_plane / (-far_plane - -near_plane),
							1.0f);
	}

	/*
	*	Calculates a perspective projection matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 Perspective(const float32 field_of_view, const float32 aspect_ratio, const float32 near_plane, const float32 far_plane) NOEXCEPT
	{
		const float32 f{ 1.0f / CatalystBaseMath::Tangent(field_of_view * 0.5f) };

		return Matrix4x4(	-(f / aspect_ratio),
							0.0f,
							0.0f,
							0.0f,

							0.0f,
							-f,
							0.0f,
							0.0f,

							0.0f,
							0.0f,
							far_plane / (near_plane - far_plane),
							-1.0f,

							0.0f,
							0.0f,
							(near_plane * far_plane) / (near_plane - far_plane),
							0.0f);
	}

	/*
	*	Calculates a reverse perspective projection matrix.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Matrix4x4 ReversePerspective(const float32 field_of_view, const float32 aspect_ratio, const float32 near_plane, const float32 far_plane) NOEXCEPT
	{
		const float32 f{ 1.0f / CatalystBaseMath::Tangent(field_of_view * 0.5f) };

		return Matrix4x4(	-(f / aspect_ratio),
							0.0f,
							0.0f,
							0.0f,

							0.0f,
							-f,
							0.0f,
							0.0f,

							0.0f,
							0.0f,
							-(far_plane / (near_plane - far_plane)) - 1.0f,
							-1.0f,

							0.0f,
							0.0f,
							-((near_plane * far_plane) / (near_plane - far_plane)),
							0.0f);
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
	FORCE_INLINE constexpr Matrix4x4(const float32 scalar) NOEXCEPT
		:
		_Matrix{ { scalar, 0.0f, 0.0f, 0.0f }, { 0.0f, scalar, 0.0f, 0.0f }, { 0.0f, 0.0f, scalar, 0.0f }, { 0.0f, 0.0f, 0.0f, scalar } }
	{

	}

	/*
	*	Constructor taking the sixteen scalars as arguments.
	*/
	FORCE_INLINE constexpr Matrix4x4(	const float32 _1_1,
										const float32 _1_2,
										const float32 _1_3,
										const float32 _1_4,
										const float32 _2_1,
										const float32 _2_2,
										const float32 _2_3,
										const float32 _2_4,
										const float32 _3_1,
										const float32 _3_2,
										const float32 _3_3,
										const float32 _3_4,
										const float32 _4_1,
										const float32 _4_2,
										const float32 _4_3,
										const float32 _4_4) NOEXCEPT
		:
		_Matrix{ { _1_1, _1_2, _1_3, _1_4 }, { _2_1, _2_2, _2_3, _2_4 }, { _3_1, _3_2, _3_3, _3_4 }, { _4_1, _4_2, _4_3, _4_4 } }
	{

	}

	/*
	*	Constructor taking the four vectors as arguments.
	*/
	FORCE_INLINE constexpr Matrix4x4(const Vector4<float32> &vector1, const Vector4<float32> &vector2, const Vector4<float32> &vector3, const Vector4<float32> &vector4) NOEXCEPT
		:
		_Matrix{ vector1, vector2, vector3, vector4 }
	{

	}

	/*
	*	Constructor taking translation, rotation and scale as arguments.
	*/
	FORCE_INLINE constexpr Matrix4x4(const Vector3<float32> &translation, const EulerAngles &rotation, const Vector3<float32> &scale) NOEXCEPT
		:
		_Matrix{ { 1.0f, 0.0f, 0.0f, 0.0f },
				 { 0.0f, 1.0f, 0.0f, 0.0f },
				 { 0.0f, 0.0f, 1.0f, 0.0f },
				 { 0.0f, 0.0f, 0.0f, 1.0f } }
	{
		//Translate this matrix.
		Translate(translation);

		//Scale this matrix.
		Scale(scale);

		//Rotate the matrix.
		Rotate(rotation);
	}

	/*
	*	Constructor taking position and a rotation quaternion as arguments.
	*/
	Matrix4x4(const Vector3<float32> &translation, const Quaternion &rotation, const Vector3<float32> &scale = VectorConstants::ONE) NOEXCEPT;

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
	FORCE_INLINE constexpr void operator*=(const float32 scalar) NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector4<float32> operator*(const Vector4<float32> &vector) const NOEXCEPT
	{
		return Vector4<float>(	(_Matrix[0]._X * vector._X) + (_Matrix[1]._X * vector._Y) + (_Matrix[2]._X * vector._Z) + (_Matrix[3]._X * vector._W),
								(_Matrix[0]._Y * vector._X) + (_Matrix[1]._Y * vector._Y) + (_Matrix[2]._Y * vector._Z) + (_Matrix[3]._Y * vector._W),
								(_Matrix[0]._Z * vector._X) + (_Matrix[1]._Z * vector._Y) + (_Matrix[2]._Z * vector._Z) + (_Matrix[3]._Z * vector._W),
								(_Matrix[0]._W * vector._X) + (_Matrix[1]._W * vector._Y) + (_Matrix[2]._W * vector._Z) + (_Matrix[3]._W * vector._W));
	}

	/*
	*	Returns the value at the given row/column index.
	*/
	FORCE_INLINE constexpr NO_DISCARD float32 AtRowColumn(const uint8 row_index, const uint8 column_index) const NOEXCEPT
	{
		return _Matrix[row_index][column_index];
	}

	/*
	*	Returns the value at the given column/row index.
	*/
	FORCE_INLINE constexpr NO_DISCARD float32 AtColumnRow(const uint8 column_index, const uint8 row_index) const NOEXCEPT
	{
		return _Matrix[row_index][column_index];
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
	RESTRICTED constexpr float32 *const RESTRICT Data() NOEXCEPT { return &(_Matrix[0]._X); }

	/*
	*	Returns the translation.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float32> GetTranslation() const NOEXCEPT
	{
		return Vector3<float32>(_Matrix[3]._X, _Matrix[3]._Y, _Matrix[3]._Z);
	}

	/*
	*	Returns the right.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float32> GetRight() const NOEXCEPT
	{
		return Vector3<float32>(_Matrix[0]._X, _Matrix[0]._Y, _Matrix[0]._Z);
	}

	/*
	*	Returns the up.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float32> GetUp() const NOEXCEPT
	{
		return Vector3<float32>(_Matrix[1]._X, _Matrix[1]._Y, _Matrix[1]._Z);
	}

	/*
	*	Returns the forward.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float32> GetForward() const NOEXCEPT
	{
		return Vector3<float32>(_Matrix[2]._X, _Matrix[2]._Y, _Matrix[2]._Z);
	}

	/*
	*	Returns the rotation.
	*/
	FORCE_INLINE NO_DISCARD EulerAngles GetRotation() const NOEXCEPT
	{
		EulerAngles out_rotation;

		out_rotation._Yaw = CatalystBaseMath::ArcSine(CatalystBaseMath::Clamp(-AtColumnRow(0, 2), 0.0f, 1.0f));

		const float32 C{ CatalystBaseMath::Cosine(out_rotation._Yaw) };

		if(CatalystBaseMath::Absolute(C) > FLOAT32_EPSILON)
		{
			float32 tan_x{ AtColumnRow(2, 2) / C };
			float32 tan_y{ AtColumnRow(1, 2) / C };

			out_rotation._Roll = CatalystBaseMath::ArcTangent(tan_y, tan_x);

			tan_x = AtColumnRow(0, 0) / C;
			tan_y = AtColumnRow(0, 1) / C;

			out_rotation._Pitch = -CatalystBaseMath::ArcTangent(tan_y, tan_x);
		}

		else
		{
			out_rotation._Roll = 0.0f;

			const float32 tan_x{ AtColumnRow(1, 1) };
			const float32 tan_y{ -AtColumnRow(1, 0) };

			out_rotation._Pitch = -CatalystBaseMath::ArcTangent(tan_y, tan_x);
		}

		return out_rotation;
	}

	/*
	*	Returns the scale.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3<float32> GetScale() const NOEXCEPT
	{
		return Vector3<float32>(_Matrix[0]._X, _Matrix[1]._Y, _Matrix[2]._Z);
	}

	/*
	*	Sets the translation.
	*/
	FORCE_INLINE constexpr void SetTranslation(const Vector3<float32> &newTranslation) NOEXCEPT
	{
		_Matrix[3]._X = newTranslation._X;
		_Matrix[3]._Y = newTranslation._Y;
		_Matrix[3]._Z = newTranslation._Z;

		Verify();
	}

	/*
	*	Sets the scale.
	*/
	FORCE_INLINE constexpr void SetScale(const Vector3<float32> &newScale) NOEXCEPT
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
		float32 Coef00 = _Matrix[2]._Z * _Matrix[3]._W - _Matrix[3]._Z * _Matrix[2]._W;
		float32 Coef02 = _Matrix[1]._Z * _Matrix[3]._W - _Matrix[3]._Z * _Matrix[1]._W;
		float32 Coef03 = _Matrix[1]._Z * _Matrix[2]._W - _Matrix[2]._Z * _Matrix[1]._W;

		float32 Coef04 = _Matrix[2]._Y * _Matrix[3]._W - _Matrix[3]._Y * _Matrix[2]._W;
		float32 Coef06 = _Matrix[1]._Y * _Matrix[3]._W - _Matrix[3]._Y * _Matrix[1]._W;
		float32 Coef07 = _Matrix[1]._Y * _Matrix[2]._W - _Matrix[2]._Y * _Matrix[1]._W;

		float32 Coef08 = _Matrix[2]._Y * _Matrix[3]._Z - _Matrix[3]._Y * _Matrix[2]._Z;
		float32 Coef10 = _Matrix[1]._Y * _Matrix[3]._Z - _Matrix[3]._Y * _Matrix[1]._Z;
		float32 Coef11 = _Matrix[1]._Y * _Matrix[2]._Z - _Matrix[2]._Y * _Matrix[1]._Z;

		float32 Coef12 = _Matrix[2]._X * _Matrix[3]._W - _Matrix[3]._X * _Matrix[2]._W;
		float32 Coef14 = _Matrix[1]._X * _Matrix[3]._W - _Matrix[3]._X * _Matrix[1]._W;
		float32 Coef15 = _Matrix[1]._X * _Matrix[2]._W - _Matrix[2]._X * _Matrix[1]._W;

		float32 Coef16 = _Matrix[2]._X * _Matrix[3]._Z - _Matrix[3]._X * _Matrix[2]._Z;
		float32 Coef18 = _Matrix[1]._X * _Matrix[3]._Z - _Matrix[3]._X * _Matrix[1]._Z;
		float32 Coef19 = _Matrix[1]._X * _Matrix[2]._Z - _Matrix[2]._X * _Matrix[1]._Z;

		float32 Coef20 = _Matrix[2]._X * _Matrix[3]._Y - _Matrix[3]._X * _Matrix[2]._Y;
		float32 Coef22 = _Matrix[1]._X * _Matrix[3]._Y - _Matrix[3]._X * _Matrix[1]._Y;
		float32 Coef23 = _Matrix[1]._X * _Matrix[2]._Y - _Matrix[2]._X * _Matrix[1]._Y;

		Vector4<float32> Fac0(Coef00, Coef00, Coef02, Coef03);
		Vector4<float32> Fac1(Coef04, Coef04, Coef06, Coef07);
		Vector4<float32> Fac2(Coef08, Coef08, Coef10, Coef11);
		Vector4<float32> Fac3(Coef12, Coef12, Coef14, Coef15);
		Vector4<float32> Fac4(Coef16, Coef16, Coef18, Coef19);
		Vector4<float32> Fac5(Coef20, Coef20, Coef22, Coef23);

		Vector4<float32> Vec0(_Matrix[1]._X, _Matrix[0]._X, _Matrix[0]._X, _Matrix[0]._X);
		Vector4<float32> Vec1(_Matrix[1]._Y, _Matrix[0]._Y, _Matrix[0]._Y, _Matrix[0]._Y);
		Vector4<float32> Vec2(_Matrix[1]._Z, _Matrix[0]._Z, _Matrix[0]._Z, _Matrix[0]._Z);
		Vector4<float32> Vec3(_Matrix[1]._W, _Matrix[0]._W, _Matrix[0]._W, _Matrix[0]._W);

		Vector4<float32> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		Vector4<float32> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		Vector4<float32> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		Vector4<float32> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		Vector4<float32> SignA(+1, -1, +1, -1);
		Vector4<float32> SignB(-1, +1, -1, +1);
		Matrix4x4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

		Vector4<float32> Row0(Inverse._Matrix[0]._X, Inverse._Matrix[1]._X, Inverse._Matrix[2]._X, Inverse._Matrix[3]._X);

		Vector4<float32> Dot0(_Matrix[0] * Row0);
		float32 Dot1 = (Dot0._X + Dot0._Y) + (Dot0._Z + Dot0._W);

		float32 OneOverDeterminant = 1.0f / Dot1;

		Inverse *= OneOverDeterminant;

		_Matrix[0] = Inverse._Matrix[0];
		_Matrix[1] = Inverse._Matrix[1];
		_Matrix[2] = Inverse._Matrix[2];
		_Matrix[3] = Inverse._Matrix[3];

		Verify();
	}

	/*
	*	Translates this matrix.
	*/
	FORCE_INLINE constexpr void Translate(const Vector3<float32> &translation) NOEXCEPT
	{
		_Matrix[3] = _Matrix[0] * translation[0] + _Matrix[1] * translation[1] + _Matrix[2] * translation[2] + _Matrix[3];

		Verify();
	}

	/*
	*	Rotates this matrix.
	*/
	FORCE_INLINE constexpr void Rotate(const EulerAngles &rotation) NOEXCEPT
	{
		//Rotate on all axes.
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			if (rotation[i] == 0.0f)
			{
				continue;
			}

			const float32 angle{ rotation[i] };
			const float32 c{ CatalystBaseMath::Cosine(angle) };
			const float32 s{ CatalystBaseMath::Sine(angle) };
			
			const Vector3<float32> axis{ static_cast<float32>(i == 0), static_cast<float32>(i == 1), static_cast<float32>(i == 2) };

			const Vector3<float32> temp{ (1.0f - c) * axis };

			Matrix4x4 rotation_matrix;

			rotation_matrix._Matrix[0][0] = c + temp[0] * axis[0];
			rotation_matrix._Matrix[0][1] = temp[0] * axis[1] + s * axis[2];
			rotation_matrix._Matrix[0][2] = temp[0] * axis[2] - s * axis[1];

			rotation_matrix._Matrix[1][0] = temp[1] * axis[0] - s * axis[2];
			rotation_matrix._Matrix[1][1] = c + temp[1] * axis[1];
			rotation_matrix._Matrix[1][2] = temp[1] * axis[2] + s * axis[0];

			rotation_matrix._Matrix[2][0] = temp[2] * axis[0] + s * axis[1];
			rotation_matrix._Matrix[2][1] = temp[2] * axis[1] - s * axis[0];
			rotation_matrix._Matrix[2][2] = c + temp[2] * axis[2];

			*this = *this * rotation_matrix;
		}

		Verify();
	}

	/*
	*	Rotates this matrix.
	*/
	void Rotate(const Quaternion &rotation) NOEXCEPT;

	/*
	*	Scales this matrix.
	*/
	FORCE_INLINE constexpr void Scale(const Vector3<float32> &scale) NOEXCEPT
	{
		_Matrix[0] = _Matrix[0] * scale[0];
		_Matrix[1] = _Matrix[1] * scale[1];
		_Matrix[2] = _Matrix[2] * scale[2];

		Verify();
	}

	/*
	*	Transposes this matrix.
	*/
	FORCE_INLINE constexpr void Transpose() NOEXCEPT
	{
		//Diagional stays the same, swap all other elements.
		Swap(&_Matrix[0][1], &_Matrix[1][0]);
		Swap(&_Matrix[0][2], &_Matrix[2][0]);
		Swap(&_Matrix[0][3], &_Matrix[3][0]);
		Swap(&_Matrix[1][2], &_Matrix[2][1]);
		Swap(&_Matrix[1][3], &_Matrix[3][1]);
		Swap(&_Matrix[2][3], &_Matrix[3][2]);

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