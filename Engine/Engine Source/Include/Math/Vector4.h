#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Forward declarations.
class Vector3;

class Vector4 final
{

public:

	//The X component.
	float X;

	//The Y component.
	float Y;

	//The Z component.
	float Z;

	//The W component.
	float W;

	/*
	*	Default constructor.
	*/
	Vector4() NOEXCEPT;

	/*
	*	Constructor taking a single float as argument.
	*/
	Vector4(const float newValue) NOEXCEPT;

	/*
	*	Constructor taking the X, Y, Z and W components as arguments.
	*/
	Vector4(const float newX, const float newY, const float newZ, const float newW) NOEXCEPT;

	/*
	*	Constructor taking a Vector3 as argument.
	*/
	Vector4(const Vector3 &otherVector) NOEXCEPT;

	/*
	*	Vector4 by scalar multiplication overload.
	*/
	Vector4 operator*(const float scalar) const NOEXCEPT
	{
		return Vector4(X * scalar, Y * scalar, Z * scalar, W * scalar);
	}

	/*
	*	Vector4 by Vector4 addition overload.
	*/
	Vector4 operator+(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(X + vector.X, Y + vector.Y, Z + vector.Z, W + vector.W);
	}

	/*
	*	Vector4 by Vector4 addition assignment overload.
	*/
	const Vector4& operator+=(const Vector4 &otherVector) NOEXCEPT
	{
		X += otherVector.X;
		Y += otherVector.Y;
		Z += otherVector.Z;
		W += otherVector.W;

		return *this;
	}

	/*
	*	Vector4 by Vector4 multiplication overload.
	*/
	Vector4 operator*(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(X * vector.X, Y * vector.Y, Z * vector.Z, W * vector.W);
	}

	/*
	*	Vector4 by Vector4 subtraction overload.
	*/
	Vector4 operator-(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(X - vector.X, Y - vector.Y, Z - vector.Z, W - vector.W);
	}

};