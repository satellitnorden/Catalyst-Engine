#pragma once

//Static variable definitions.
template <typename Type>
const Vector3<Type> Vector3<Type>::FORWARD{ 0.0f, 0.0f, -1.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::BACKWARD{ 0.0f, 0.0f, 1.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::UP{ 0.0f, 1.0f, 0.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::DOWN{ 0.0f, -1.0f, 0.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::RIGHT{ 1.0f, 0.0f, 0.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::LEFT{ -1.0f, 0.0f, 0.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::ZERO{ 0.0f, 0.0f, 0.0f };
template <typename Type>
const Vector3<Type> Vector3<Type>::ONE{ 1.0f, 1.0f, 1.0f };

/*
*	Scalar by Vector3 multiplication operator overload.
*/
template <typename Type>
constexpr static Vector3<Type> operator*(const float scalar, const Vector3<Type> &vector) NOEXCEPT
{
	return Vector3<Type>{ vector._X * scalar, vector._Y * scalar, vector._Z * scalar };
}