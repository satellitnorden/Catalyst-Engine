#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class Vector4 final
{

public:

	//The X component.
	float _X;

	//The Y component.
	float _Y;

	//The Z component.
	float _Z;

	//The W component.
	float _W;

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
		_X(0.0f),
		_Y(0.0f),
		_Z(0.0f),
		_W(0.0f)
	{

	}

	/*
	*	Constructor taking a single float as argument.
	*/
	constexpr Vector4(const float newValue) NOEXCEPT
		:
		_X(newValue),
		_Y(newValue),
		_Z(newValue),
		_W(newValue)
	{

	}

	/*
	*	Constructor taking the X, Y, Z and W components as arguments.
	*/
	constexpr Vector4(const float newX, const float newY, const float newZ, const float newW) NOEXCEPT

		:
		_X(newX),
		_Y(newY),
		_Z(newZ),
		_W(newW)
	{

	}

	/*
	*	Constructor taking a Vector3 as argument.
	*/
	constexpr Vector4(const Vector3 &otherVector) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z),
		_W(1.0f)
	{

	}

	/*
	*	Constructor taking a Vector3 and a scalar as argument.
	*/
	constexpr Vector4(const Vector3 &otherVector, const float scalar) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z),
		_W(scalar)
	{

	}

	/*
	*	Vector4 by scalar multiplication overload.
	*/
	constexpr Vector4 operator*(const float scalar) const NOEXCEPT
	{
		return Vector4(_X * scalar, _Y * scalar, _Z * scalar, _W * scalar);
	}

	/*
	*	Vector4 by scalar multiplication assignment overload.
	*/
	constexpr void operator*=(const float scalar) NOEXCEPT
	{
		_X *= scalar;
		_Y *= scalar;
		_Z *= scalar;
		_W *= scalar;
	}

	/*
	*	Vector4 by Vector4 addition overload.
	*/
	constexpr Vector4 operator+(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(_X + vector._X, _Y + vector._Y, _Z + vector._Z, _W + vector._W);
	}

	/*
	*	Vector4 by Vector4 addition assignment overload.
	*/
	constexpr void operator+=(const Vector4 &otherVector) NOEXCEPT
	{
		_X += otherVector._X;
		_Y += otherVector._Y;
		_Z += otherVector._Z;
		_W += otherVector._W;
	}

	/*
	*	Vector4 by Vector4 multiplication overload.
	*/
	constexpr Vector4 operator*(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(_X * vector._X, _Y * vector._Y, _Z * vector._Z, _W * vector._W);
	}

	/*
	*	Vector4 by Vector4 subtraction overload.
	*/
	constexpr Vector4 operator-(const Vector4 &vector) const NOEXCEPT
	{
		return Vector4(_X - vector._X, _Y - vector._Y, _Z - vector._Z, _W - vector._W);
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
		return (_X * _X) + (_Y * _Y) + (_Z * _Z) + (_W * _W);
	}

	/*
	*	Returns the length of the X, Y and Z components of the vector.
	*/
	constexpr float LengthXYZ() const NOEXCEPT
	{
		return CatalystMath::SquareRoot((_X * _X) + (_Y * _Y) + (_Z * _Z));
	}

	/*
	*	Normalizes the vector.
	*/
	constexpr void Normalize() NOEXCEPT
	{
		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		_X *= inverseLength;
		_Y *= inverseLength;
		_Z *= inverseLength;
		_W *= inverseLength;
	}

};