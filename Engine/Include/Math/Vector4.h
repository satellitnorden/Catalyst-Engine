#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

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
	*	Linearly interpolate between two Vector4 values.
	*/
	constexpr static Vector4 LinearlyInterpolate(const Vector4 &valueA, const Vector4 &valueB, const float alpha) NOEXCEPT
	{
		return (valueA * (1.0f - alpha)) + (valueB * alpha);
	}

	/*
	*	Default constructor.
	*/
	constexpr Vector4() NOEXCEPT
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
	constexpr Vector4(const float newValue) NOEXCEPT
		:
		X(newValue),
		Y(newValue),
		Z(newValue),
		W(newValue)
	{

	}

	/*
	*	Constructor taking the X, Y, Z and W components as arguments.
	*/
	constexpr Vector4(const float newX, const float newY, const float newZ, const float newW) NOEXCEPT

		:
		X(newX),
		Y(newY),
		Z(newZ),
		W(newW)
	{

	}

	/*
	*	Constructor taking a Vector3 as argument.
	*/
	constexpr Vector4(const Vector3 &otherVector) NOEXCEPT
		:
		X(otherVector.X),
		Y(otherVector.Y),
		Z(otherVector.Z),
		W(1.0f)
	{

	}

	/*
	*	Constructor taking a Vector3 and a scalar as argument.
	*/
	constexpr Vector4(const Vector3 &otherVector, const float scalar) NOEXCEPT
		:
		X(otherVector.X),
		Y(otherVector.Y),
		Z(otherVector.Z),
		W(scalar)
	{

	}

	/*
	*	Vector4 by scalar multiplication overload.
	*/
	constexpr Vector4 operator*(const float scalar) const NOEXCEPT
	{
		return Vector4(X * scalar, Y * scalar, Z * scalar, W * scalar);
	}

	/*
	*	Vector4 by scalar multiplication assignment overload.
	*/
	constexpr void operator*=(const float scalar) NOEXCEPT
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		W *= scalar;
	}

	/*
	*	Vector4 by Vector4 addition overload.
	*/
	constexpr Vector4 operator+(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(X + vector.X, Y + vector.Y, Z + vector.Z, W + vector.W);
	}

	/*
	*	Vector4 by Vector4 addition assignment overload.
	*/
	constexpr void operator+=(const Vector4 &otherVector) NOEXCEPT
	{
		X += otherVector.X;
		Y += otherVector.Y;
		Z += otherVector.Z;
		W += otherVector.W;
	}

	/*
	*	Vector4 by Vector4 multiplication overload.
	*/
	constexpr Vector4 operator*(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(X * vector.X, Y * vector.Y, Z * vector.Z, W * vector.W);
	}

	/*
	*	Vector4 by Vector4 subtraction overload.
	*/
	constexpr Vector4 operator-(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(X - vector.X, Y - vector.Y, Z - vector.Z, W - vector.W);
	}

	/*
	*	Returns the length of the vector.
	*/
	constexpr float Length() const NOEXCEPT
	{
		return CatalystMath::SquareRoot(LengthSquared());
	}

	/*
	*	Returns the length of the vector squared.
	*/
	constexpr float LengthSquared() const NOEXCEPT
	{
		return (X * X) + (Y * Y) + (Z * Z) + (W * W);
	}

	/*
	*	Returns the length of the X, Y and Z components of the vector.
	*/
	constexpr float LengthXYZ() const NOEXCEPT
	{
		return CatalystMath::SquareRoot((X * X) + (Y * Y) + (Z * Z));
	}

	/*
	*	Normalizes the vector.
	*/
	constexpr void Normalize() NOEXCEPT
	{
		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		X *= inverseLength;
		Y *= inverseLength;
		Z *= inverseLength;
		W *= inverseLength;
	}

};