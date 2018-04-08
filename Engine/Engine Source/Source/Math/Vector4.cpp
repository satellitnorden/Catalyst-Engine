//Header file.
#include <Math/Vector4.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector3.h>

/*
*	Default constructor.
*/
Vector4::Vector4() NOEXCEPT
	:
	X(0.0f),
	Y(0.0f),
	Z(0.0f),
	W(0.0f)
{

}

/*
*	Constructor taking a single float as argument.
*/
Vector4::Vector4(const float newValue) NOEXCEPT
	:
	X(newValue),
	Y(newValue),
	Z(newValue),
	W(newValue)
{

}

/*
*	Constructor taking a Vector3 as argument.
*/
Vector4::Vector4(const Vector3 &otherVector) NOEXCEPT
	:
	X(otherVector.X),
	Y(otherVector.Y),
	Z(otherVector.Z),
	W(1.0f)
{

}

/*
*	Constructor taking the X, Y, Z and W components as arguments.
*/
Vector4::Vector4(const float newX, const float newY, const float newZ, const float newW) NOEXCEPT
	:
	X(newX),
	Y(newY),
	Z(newZ),
	W(newW)
{

}

/*
*	Returns the length of the vector.
*/
float Vector4::Length() const NOEXCEPT
{
	return CatalystMath::SquareRoot((X * X) + (Y * Y) + (Z * Z) + (W * W));
}

/*
*	Returns the length of the X, Y and Z components of the vector.
*/
float Vector4::LengthXYZ() const NOEXCEPT
{
	return CatalystMath::SquareRoot((X * X) + (Y * Y) + (Z * Z));
}