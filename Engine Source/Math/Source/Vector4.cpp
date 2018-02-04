//Header file.
#include <Vector4.h>

//Math.
#include <Vector3.h>

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