#pragma once

/*
*	Scalar by Vector3 multiplication operator overload.
*/
static Vector3 operator*(const float scalar, const Vector3 &vector) NOEXCEPT
{
	return Vector3{ vector._X * scalar, vector._Y * scalar, vector._Z * scalar };
}