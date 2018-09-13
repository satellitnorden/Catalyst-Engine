#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>

class Vector2 final
{

public:

	//The X component.
	float _X;

	//The Y component.
	float _Y;

	/*
	*	Given a vector, return it's length.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Length(const Vector2 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(vector._X * vector._X + vector._Y * vector._Y);
	}

	/*
	*	Given a vector, return it's length squared.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthSquared(const Vector2 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Y * vector._Y;
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Vector2() NOEXCEPT
		:
		_X(0.0f),
		_Y(0.0f)
	{

	}

	/*
	*	Constructor taking X and Y arguments.
	*/
	FORCE_INLINE constexpr Vector2(const float newX, const float newY) NOEXCEPT
		:
		_X(newX),
		_Y(newY)
	{

	}

	/*
	*	Vector2 by scalar addition operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator+(const float scalar) const NOEXCEPT
	{
		return Vector2(_X + scalar, _Y + scalar);
	}

	/*
	*	Vector2 by scalar addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const float scalar) NOEXCEPT
	{
		_X += scalar;
		_Y += scalar;
	}

	/*
	*	Vector2 by scalar subtraction operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator-(const float scalar) const NOEXCEPT
	{
		return Vector2(_X - scalar, _Y - scalar);
	}

	/*
	*	Vector2 by scalar subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const float scalar) NOEXCEPT
	{
		_X -= scalar;
		_Y -= scalar;
	}

	/*
	*	Vector2 by scalar multiplication operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator*(const float scalar) const NOEXCEPT
	{
		return Vector2(_X * scalar, _Y * scalar);
	}

	/*
	*	Vector2 by scalar multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const float scalar) NOEXCEPT
	{
		_X *= scalar;
		_Y *= scalar;
	}

	/*
	*	Vector2 by scalar division operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator/(const float scalar) const NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		return Vector2(_X * inverseScalar, _Y * inverseScalar);
	}

	/*
	*	Vector2 by scalar division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const float scalar) NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		_X *= inverseScalar;
		_Y *= inverseScalar;
	}

	/*
	*	Vector2 by Vector2 addition operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator+(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(_X + otherVector._X, _Y + otherVector._Y);
	}

	/*
	*	Vector2 by Vector2 addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const Vector2 &otherVector) NOEXCEPT
	{
		_X += otherVector._X;
		_Y += otherVector._Y;
	}

	/*
	*	Vector2 by Vector2 subtraction operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator-(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(_X - otherVector._X, _Y - otherVector._Y);
	}

	/*
	*	Vector2 by Vector2 subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const Vector2 &otherVector) NOEXCEPT
	{
		_X -= otherVector._X;
		_Y -= otherVector._Y;
	}

	/*
	*	Vector2 by Vector2 multiplication operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator*(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(_X * otherVector._X, _Y * otherVector._Y);
	}

	/*
	*	Vector2 by Vector2 multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const Vector2 &otherVector) NOEXCEPT
	{
		_X *= otherVector._X;
		_Y *= otherVector._Y;
	}

	/*
	*	Vector2 by Vector2 division operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector2 operator/(const Vector2 &otherVector) const NOEXCEPT
	{
		return Vector2(_X / otherVector._X, _Y / otherVector._Y);
	}

	/*
	*	Vector2 by Vector2 division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const Vector2 &otherVector) NOEXCEPT
	{
		_X /= otherVector._X;
		_Y /= otherVector._Y;
	}

	/*
	*	Returns a pointer to this vector's data, const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const float* const RESTRICT Data() const NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD float* RESTRICT Data() NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns the length of the vector.
	*/
	FORCE_INLINE constexpr NO_DISCARD float Length() const NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquared());
	}

	/*
	*	Returns the length of the vector squared.
	*/
	FORCE_INLINE constexpr NO_DISCARD float LengthSquared() const NOEXCEPT
	{
		return (_X * _X) + (_Y * _Y);
	}

	/*
	*	Normalize the vector to a unit vector.
	*/
	FORCE_INLINE constexpr void Normalize() NOEXCEPT
	{
		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		_X *= inverseLength;
		_Y *= inverseLength;
	}

	/*
	*	Given an amount of degrees, rotate the vector.
	*/
	FORCE_INLINE constexpr void Rotate(const float degrees) NOEXCEPT
	{
		const float theta{ CatalystBaseMath::DegreesToRadians(degrees) };

		const float cosine{ CatalystBaseMath::CosineRadians(theta) };
		const float sine{ CatalystBaseMath::SineDegrees(theta) };

		const float newX{ _X * cosine - _Y * sine };
		const float newY{ _X * cosine + _Y * sine };

		_X = newX;
		_Y = newY;
	}

};