#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>

class Vector2 final
{

public:

	//The X component.
	float X;

	//The Y component.
	float Y;

	/*
	*	Given a vector, return it's length.
	*/
	static constexpr float Length(const Vector2 &vector) NOEXCEPT { return CatalystMath::SquareRoot(vector.X * vector.X + vector.Y * vector.Y); }

	/*
	*	Given a vector, return it's length squared.
	*/
	static constexpr float LengthSquared(const Vector2 &vector) NOEXCEPT { return vector.X * vector.X + vector.Y * vector.Y; }

	/*
	*	Default constructor.
	*/
	constexpr Vector2() NOEXCEPT
		:
		X(0.0f),
		Y(0.0f)
	{

	}

	/*
	*	Constructor taking X and Y arguments.
	*/
	constexpr Vector2(const float newX, const float newY) NOEXCEPT
		:
		X(newX),
		Y(newY)
	{

	}

	/*
	*	Vector2 by scalar addition operator overload.
	*/
	constexpr Vector2 operator+(const float scalar) const NOEXCEPT { return Vector2(X + scalar, Y + scalar); }

	/*
	*	Vector2 by scalar addition assignment operator overload.
	*/
	constexpr void operator+=(const float scalar) NOEXCEPT
	{
		X += scalar;
		Y += scalar;
	}

	/*
	*	Vector2 by scalar subtraction operator overload.
	*/
	constexpr Vector2 operator-(const float scalar) const NOEXCEPT { return Vector2(X - scalar, Y - scalar); }

	/*
	*	Vector2 by scalar subtraction assignment operator overload.
	*/
	constexpr void operator-=(const float scalar) NOEXCEPT
	{
		X -= scalar;
		Y -= scalar;
	}

	/*
	*	Vector2 by scalar multiplication operator overload.
	*/
	constexpr Vector2 operator*(const float scalar) const NOEXCEPT { return Vector2(X * scalar, Y * scalar); }

	/*
	*	Vector2 by scalar multiplication assignment operator overload.
	*/
	constexpr void operator*=(const float scalar) NOEXCEPT
	{
		X *= scalar;
		Y *= scalar;
	}

	/*
	*	Vector2 by scalar division operator overload.
	*/
	constexpr Vector2 operator/(const float scalar) const NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		return Vector2(X * inverseScalar, Y * inverseScalar);
	}

	/*
	*	Vector2 by scalar division assignment operator overload.
	*/
	constexpr void operator/=(const float scalar) NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		X *= inverseScalar;
		Y *= inverseScalar;
	}

	/*
	*	Vector2 by Vector2 addition operator overload.
	*/
	constexpr Vector2 operator+(const Vector2 &otherVector) const NOEXCEPT { return Vector2(this->X + otherVector.X, this->Y + otherVector.Y); }

	/*
	*	Vector2 by Vector2 addition assignment operator overload.
	*/
	constexpr void operator+=(const Vector2 &otherVector) NOEXCEPT
	{
		this->X += otherVector.X;
		this->Y += otherVector.Y;
	}

	/*
	*	Vector2 by Vector2 subtraction operator overload.
	*/
	constexpr Vector2 operator-(const Vector2 &otherVector) const NOEXCEPT { return Vector2(this->X - otherVector.X, this->Y - otherVector.Y); }

	/*
	*	Vector2 by Vector2 subtraction assignment operator overload.
	*/
	constexpr void operator-=(const Vector2 &otherVector) NOEXCEPT
	{
		this->X -= otherVector.X;
		this->Y -= otherVector.Y;
	}

	/*
	*	Vector2 by Vector2 multiplication operator overload.
	*/
	constexpr Vector2 operator*(const Vector2 &otherVector) const NOEXCEPT { return Vector2(this->X * otherVector.X, this->Y * otherVector.Y); }

	/*
	*	Vector2 by Vector2 multiplication assignment operator overload.
	*/
	constexpr void operator*=(const Vector2 &otherVector) NOEXCEPT
	{
		this->X *= otherVector.X;
		this->Y *= otherVector.Y;
	}

	/*
	*	Vector2 by Vector2 division operator overload.
	*/
	constexpr Vector2 operator/(const Vector2 &otherVector) const NOEXCEPT { return Vector2(this->X / otherVector.X, this->Y / otherVector.Y); }

	/*
	*	Vector2 by Vector2 division assignment operator overload.
	*/
	constexpr void operator/=(const Vector2 &otherVector) NOEXCEPT
	{
		this->X /= otherVector.X;
		this->Y /= otherVector.Y;
	}

	/*
	*	Returns a pointer to this vector's data.
	*/
	RESTRICTED constexpr float* Data() NOEXCEPT { return &X; }

	/*
	*	Returns the length of the vector.
	*/
	constexpr float Length() const NOEXCEPT { return CatalystMath::SquareRoot((X * X) + (Y * Y)); }

	/*
	*	Returns the length of the vector squared.
	*/
	constexpr float LengthSquared() const NOEXCEPT { return (X * X) + (Y * Y); }

	/*
	*	Normalize the vector to a unit vector.
	*/
	constexpr void Normalize() NOEXCEPT
	{
		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		X *= inverseLength;
		Y *= inverseLength;
	}

	/*
	*	Given an amount of degrees, rotate the vector.
	*/
	void Rotate(const float degrees) NOEXCEPT
	{
		const float theta{ CatalystMath::DegreesToRadians(degrees) };

		const float cosine{ CatalystMath::CosineRadians(theta) };
		const float sine{ CatalystMath::SineDegrees(theta) };

		const float newX{ X * cosine - Y * sine };
		const float newY{ X * cosine - Y * sine };

		X = newX;
		Y = newY;
	}

};