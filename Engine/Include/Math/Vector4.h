#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

template <typename Type>
class Vector4 final
{

public:

	union
	{
		struct
		{
			//The X component.
			Type _X;

			//The Y component.
			Type _Y;

			//The Z component.
			Type _Z;

			//The W component.
			Type _W;
		};

		//The data.
		Type _Data[4];

	};

	/*
	*	Linearly interpolate between two Vector4 values.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector4 LinearlyInterpolate(const Vector4 &valueA, const Vector4 &valueB, const float alpha) NOEXCEPT
	{
		return (valueA * (1.0f - alpha)) + (valueB * alpha);
	}

	/*
	*	Default constructor.
	*/
	constexpr Vector4() NOEXCEPT
		:
		_X(static_cast<Type>(0)),
		_Y(static_cast<Type>(0)),
		_Z(static_cast<Type>(0)),
		_W(static_cast<Type>(0))
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
	constexpr Vector4(const Vector3<float> &otherVector) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z),
		_W(static_cast<Type>(1))
	{

	}

	/*
	*	Constructor taking a Vector3 and a scalar as argument.
	*/
	constexpr Vector4(const Vector3<float> &otherVector, const float scalar) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z),
		_W(scalar)
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	Type operator[](const uint64 index) const NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	Type& operator[](const uint64 index)  NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Vector4 by scalar addition operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator+(const float scalar) const NOEXCEPT
	{
		return Vector4{ _X + scalar, _Y + scalar, _Z + scalar, _W + scalar };
	}

	/*
	*	Vector4 by scalar addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const float scalar) NOEXCEPT
	{
		_X += scalar;
		_Y += scalar;
		_Z += scalar;
		_W += scalar;
	}

	/*
	*	Vector4 by scalar subtraction operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator-(const float scalar) const NOEXCEPT
	{
		return Vector4{ _X - scalar, _Y - scalar, _Z - scalar, _W - scalar };
	}

	/*
	*	Vector4 by scalar subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const float scalar) NOEXCEPT
	{
		_X -= scalar;
		_Y -= scalar;
		_Z -= scalar;
		_W -= scalar;
	}

	/*
	*	Vector4 by scalar multiplication operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator*(const float scalar) const NOEXCEPT
	{
		return Vector4{ _X * scalar, _Y * scalar, _Z * scalar, _W * scalar };
	}

	/*
	*	Vector4 by scalar multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const float scalar) NOEXCEPT
	{
		_X *= scalar;
		_Y *= scalar;
		_Z *= scalar;
		_W *= scalar;
	}

	/*
	*	Vector4 by scalar division operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator/(const float scalar) const NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		return Vector4{ _X * inverseScalar, _Y * inverseScalar, _Z * inverseScalar, _W * inverseScalar };
	}

	/*
	*	Vector4 by scalar division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const float scalar) NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		_X *= inverseScalar;
		_Y *= inverseScalar;
		_Z *= inverseScalar;
		_W *= inverseScalar;
	}

	/*
	*	Vector4 by Vector4 addition operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator+(const Vector4 &otherVector) const NOEXCEPT
	{
		return Vector4{ _X + otherVector._X, _Y + otherVector._Y, _Z + otherVector._Z, _W + otherVector._W };
	}

	/*
	*	Vector4 by Vector4 addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const Vector4 &otherVector) NOEXCEPT
	{
		_X += otherVector._X;
		_Y += otherVector._Y;
		_Z += otherVector._Z;
		_W += otherVector._W;
	}

	/*
	*	Vector4 by Vector4 subtraction operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator-(const Vector4 &otherVector) const NOEXCEPT
	{
		return Vector4{ _X - otherVector._X, _Y - otherVector._Y, _Z - otherVector._Z, _W - otherVector._W };
	}

	/*
	*	Vector4 by Vector4 subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const Vector4 &otherVector) NOEXCEPT
	{
		_X -= otherVector._X;
		_Y -= otherVector._Y;
		_Z -= otherVector._Z;
		_W -= otherVector._W;
	}

	/*
	*	Vector4 by Vector4 multiplication operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator*(const Vector4 &otherVector) const NOEXCEPT
	{
		return Vector4{ _X * otherVector._X, _Y * otherVector._Y, _Z * otherVector._Z, _W * otherVector._W };
	}

	/*
	*	Vector4 by Vector4 multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const Vector4 &otherVector) NOEXCEPT
	{
		_X *= otherVector._X;
		_Y *= otherVector._Y;
		_Z *= otherVector._Z;
		_W *= otherVector._W;
	}

	/*
	*	Vector4 by Vector4 division operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector4 operator/(const Vector4 &otherVector) const NOEXCEPT
	{
		return Vector4{ _X / otherVector._X, _Y / otherVector._Y, _Z / otherVector._Z, _W / otherVector._W };
	}

	/*
	*	Vector4 by Vector4 division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const Vector4 &otherVector) NOEXCEPT
	{
		_X /= otherVector._X;
		_Y /= otherVector._Y;
		_Z /= otherVector._Z;
		_W /= otherVector._W;
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
		return (_X * _X) + (_Y * _Y) + (_Z * _Z) + (_W * _W);
	}

	/*
	*	Returns the length of the X, Y and Z components of the vector.
	*/
	FORCE_INLINE constexpr NO_DISCARD float LengthXYZ() const NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot((_X * _X) + (_Y * _Y) + (_Z * _Z));
	}

	/*
	*	Normalizes the vector.
	*/
	FORCE_INLINE constexpr void Normalize() NOEXCEPT
	{
		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		_X *= inverseLength;
		_Y *= inverseLength;
		_Z *= inverseLength;
		_W *= inverseLength;
	}

};