#pragma once

/*
*	Scalar by Vector3 division operator overload.
*/
template <typename TYPE>
FORCE_INLINE constexpr static Vector2<TYPE> operator/(const float scalar, const Vector2<TYPE> &vector) NOEXCEPT
{
	return Vector2<TYPE>{ scalar / vector._X, scalar / vector._Y };
}

/*
*	Scalar by Vector3 multiplication operator overload.
*/
template <typename TYPE>
FORCE_INLINE constexpr static Vector3<TYPE> operator*(const float scalar, const Vector3<TYPE> &vector) NOEXCEPT
{
	return Vector3<TYPE>{ scalar * vector._X, scalar * vector._Y, scalar * vector._Z };
}