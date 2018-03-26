#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class Vector2 final
{

public:

	//The X component.
	float X;

	//The Y component.
	float Y;

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
	constexpr Vector2 operator+(const float scalar) const NOEXCEPT
	{
		return Vector2(this->X + scalar, this->Y + scalar);
	}

	/*
	*	Vector2 by scalar addition assignment operator overload.
	*/
	constexpr void operator+=(const float scalar) NOEXCEPT
	{
		this->X += scalar;
		this->Y += scalar;
	}

	/*
	*	Vector2 by scalar subtraction operator overload.
	*/
	constexpr Vector2 operator-(const float scalar) const NOEXCEPT
	{
		return Vector2(this->X - scalar, this->Y - scalar);
	}

	/*
	*	Vector2 by scalar subtraction assignment operator overload.
	*/
	constexpr void operator-=(const float scalar) NOEXCEPT
	{
		this->X -= scalar;
		this->Y -= scalar;
	}

	/*
	*	Vector2 by scalar multiplication operator overload.
	*/
	constexpr Vector2 operator*(const float scalar) const NOEXCEPT
	{
		return Vector2(this->X * scalar, this->Y * scalar);
	}

	/*
	*	Vector2 by scalar multiplication assignment operator overload.
	*/
	constexpr void operator*=(const float scalar) NOEXCEPT
	{
		this->X *= scalar;
		this->Y *= scalar;
	}

	/*
	*	Vector2 by scalar division operator overload.
	*/
	constexpr Vector2 operator/(const float scalar) const NOEXCEPT
	{
		return Vector2(this->X / scalar, this->Y / scalar);
	}

	/*
	*	Vector2 by scalar division assignment operator overload.
	*/
	constexpr void operator/=(const float scalar) NOEXCEPT
	{
		this->X /= scalar;
		this->Y /= scalar;
	}

	/*
	*	Vector2 by Vector2 addition operator overload.
	*/
	constexpr Vector2 operator+(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(this->X + otherVector.X, this->Y + otherVector.Y);
	}

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
	constexpr Vector2 operator-(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(this->X - otherVector.X, this->Y - otherVector.Y);
	}

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
	constexpr Vector2 operator*(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(this->X * otherVector.X, this->Y * otherVector.Y);
	}

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
	constexpr Vector2 operator/(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(this->X / otherVector.X, this->Y / otherVector.Y);
	}

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
	constexpr float Length() const NOEXCEPT;

	/*
	*	Returns the length of the vector squared.
	*/
	constexpr float LengthSquared() const NOEXCEPT { return (X * X) + (Y * Y); }

	/*
	*	Normalize the vector to a unit vector.
	*/
	constexpr void Normalize() NOEXCEPT;

	/*
	*	Given an amount of degrees, rotate the vector.
	*/
	void Rotate(const float degrees) NOEXCEPT;

};