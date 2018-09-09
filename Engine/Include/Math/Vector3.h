#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>

class Vector3 final
{

public:

	//Constant vectors.
	const static Vector3 FORWARD;
	const static Vector3 UP;
	const static Vector3 RIGHT;

	//The X component.
	float _X;

	//The Y component.
	float _Y;

	//The Z component.
	float _Z;

	/*
	*	Returns the absolute value of the Vector3 passed in.
	*/
	FORCE_INLINE NO_DISCARD constexpr static Vector3 Absolute(const Vector3 &vector) NOEXCEPT
	{
		return Vector3(CatalystBaseMath::Absolute(vector._X), CatalystBaseMath::Absolute(vector._Y), CatalystBaseMath::Absolute(vector._Z));
	}

	/*
	*	Returns the dot product of two vectors.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
	{
		return firstVector._X * secondVector._X + firstVector._Y * secondVector._Y + firstVector._Z * secondVector._Z;
	}

	/*
	*	Returns the cross product of two vectors.
	*/
	FORCE_INLINE NO_DISCARD constexpr static Vector3 CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
	{
		return Vector3{ firstVector._Y * secondVector._Z - firstVector._Z * secondVector._Y, firstVector._Z * secondVector._X - firstVector._X * secondVector._Z, firstVector._X * secondVector._Y - firstVector._Y * secondVector._X };
	}

	/*
	*	Given a vector, return it's length.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float Length(const Vector3 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquared(vector));
	}

	/*
	*	Given a vector, return it's length squared.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquared(const Vector3 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Y * vector._Y + vector._Z * vector._Z;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquaredXY(const Vector3 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Y * vector._Y;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquaredXZ(const Vector3 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Z * vector._Z;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquaredYZ(const Vector3 &vector) NOEXCEPT
	{
		return vector._Y * vector._Y + vector._Z * vector._Z;
	}

	/*
	*	Linearly interpolate between two float vectors.
	*/
	FORCE_INLINE NO_DISCARD constexpr static Vector3 LinearlyInterpolate(const Vector3 &first, const Vector3 &second, const float alpha) NOEXCEPT
	{
		return Vector3(CatalystBaseMath::LinearlyInterpolate(first._X, second._X, alpha), CatalystBaseMath::LinearlyInterpolate(first._Y, second._Y, alpha), CatalystBaseMath::LinearlyInterpolate(first._Z, second._Z, alpha));
	}

	/*
	*	Given a vector, return it's unit vector counterpart.
	*/
	FORCE_INLINE NO_DISCARD constexpr static Vector3 Normalize(const Vector3 &vector) NOEXCEPT
	{
		return vector.Normalized();
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Vector3() NOEXCEPT
		:
		_X(0.0f),
		_Y(0.0f),
		_Z(0.0f)
	{

	}

	/*
	*	Constructor taking a single scalar, applying it to both X, Y and Z.
	*/
	FORCE_INLINE constexpr Vector3(const float scalar) NOEXCEPT
		:
		_X(scalar),
		_Y(scalar),
		_Z(scalar)
	{

	}

	/*
	*	Constructor taking X, Y and Z as arguments.
	*/
	FORCE_INLINE constexpr Vector3(const float newX, const float newY, const float newZ) NOEXCEPT
		:
		_X(newX),
		_Y(newY),
		_Z(newZ)
	{

	}

	/*
	*	Constructor taking a Vector2 and a float.
	*/
	FORCE_INLINE constexpr Vector3(const Vector2 &initialXY, const float initialZ) NOEXCEPT
		:
		_X(initialXY._X),
		_Y(initialXY._Y),
		_Z(initialZ)
	{

	}

	/*
	*	Copy constructor taking another vector as argument.
	*/
	FORCE_INLINE constexpr Vector3(const Vector3 &otherVector) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z)
	{

	}

	/*
	*	Assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const Vector3 &otherVector) NOEXCEPT
	{
		_X = otherVector._X;
		_Y = otherVector._Y;
		_Z = otherVector._Z;
	}

	/*
	*	Vector3 by scalar addition operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator+(const float scalar) const NOEXCEPT
	{
		return Vector3{ _X + scalar, _Y + scalar, _Z + scalar };
	}

	/*
	*	Vector3 by scalar addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const float scalar) NOEXCEPT
	{
		_X += scalar;
		_Y += scalar;
		_Z += scalar;
	}

	/*
	*	Vector3 by scalar subtraction operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator-(const float scalar) const NOEXCEPT
	{
		return Vector3{ _X - scalar, _Y - scalar, _Z - scalar };
	}

	/*
	*	Vector3 by scalar subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const float scalar) NOEXCEPT
	{
		_X -= scalar;
		_Y -= scalar;
		_Z -= scalar;
	}

	/*
	*	Vector3 by scalar multiplication operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator*(const float scalar) const NOEXCEPT
	{
		return Vector3{ _X * scalar, _Y * scalar, _Z * scalar };
	}

	/*
	*	Vector3 by scalar multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const float scalar) NOEXCEPT
	{
		_X *= scalar;
		_Y *= scalar;
		_Z *= scalar;
	}

	/*
	*	Vector3 by scalar division operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator/(const float scalar) const NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		return Vector3{ _X * inverseScalar, _Y * inverseScalar, _Z * inverseScalar };
	}

	/*
	*	Vector3 by scalar division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const float scalar) NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		_X *= inverseScalar;
		_Y *= inverseScalar;
		_Z *= inverseScalar;
	}

	/*
	*	Vector3 by Vector3 addition operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator+(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ _X + otherVector._X, _Y + otherVector._Y, _Z + otherVector._Z };
	}

	/*
	*	Vector3 by Vector3 addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const Vector3 &otherVector) NOEXCEPT
	{
		_X += otherVector._X;
		_Y += otherVector._Y;
		_Z += otherVector._Z;
	}

	/*
	*	Vector3 by Vector3 subtraction operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator-(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ _X - otherVector._X, _Y - otherVector._Y, _Z - otherVector._Z };
	}

	/*
	*	Vector3 by Vector3 subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const Vector3 &otherVector) NOEXCEPT
	{
		_X -= otherVector._X;
		_Y -= otherVector._Y;
		_Z -= otherVector._Z;
	}

	/*
	*	Vector3 by Vector3 multiplication operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator*(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ _X * otherVector._X, _Y * otherVector._Y, _Z * otherVector._Z };
	}

	/*
	*	Vector3 by Vector3 multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const Vector3 &otherVector) NOEXCEPT
	{
		_X *= otherVector._X;
		_Y *= otherVector._Y;
		_Z *= otherVector._Z;
	}

	/*
	*	Vector3 by Vector3 division operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator/(const Vector3 &otherVector) const NOEXCEPT
	{ 
		return Vector3{ _X / otherVector._X, _Y / otherVector._Y, _Z / otherVector._Z };
	}

	/*
	*	Vector3 by Vector3 division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const Vector3 &otherVector) NOEXCEPT
	{
		_X /= otherVector._X;
		_Y /= otherVector._Y;
		_Z /= otherVector._Z;
	}

	/*
	*	Returns a pointer to this vector's data, const.
	*/
	FORCE_INLINE NO_DISCARD RESTRICTED constexpr const float* const RESTRICT Data() const NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	FORCE_INLINE NO_DISCARD RESTRICTED constexpr float* RESTRICT Data() NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns true if the vector is a unit vector, false otherwise.
	*/
	FORCE_INLINE NO_DISCARD constexpr bool IsUnitVector() const NOEXCEPT
	{
		return LengthSquared() == 1.0f;
	}

	/*
	*	Returns the length of the vector.
	*/
	FORCE_INLINE NO_DISCARD constexpr float Length() const NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquared());
	}

	/*
	*	Returns the length of the vector squared.
	*/
	FORCE_INLINE NO_DISCARD constexpr float LengthSquared() const NOEXCEPT
	{
		return (_X * _X) + (_Y * _Y) + (_Z * _Z);
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
		_Z *= inverseLength;
	}

	/*
	*	Returns a normalized version of this vector.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 Normalized() const NOEXCEPT
	{
		Vector3 copy{ *this };
		copy.Normalize();

		return copy;
	}

	/*
	*	Given a rotation vector, rotate this vector.
	*/
	constexpr void Rotate(const Vector3& rotationVector) NOEXCEPT
	{
		const float xRadians{ CatalystBaseMath::DegreesToRadians(rotationVector._X) };
		const float yRadians{ CatalystBaseMath::DegreesToRadians(rotationVector._Y) };
		const float zRadians{ CatalystBaseMath::DegreesToRadians(rotationVector._Z) };

		const float xCosine{ CatalystBaseMath::CosineRadians(xRadians) };
		const float xSine{ CatalystBaseMath::SineRadians(xRadians) };

		const float yCosine{ CatalystBaseMath::CosineRadians(yRadians) };
		const float ySine{ CatalystBaseMath::SineRadians(yRadians) };

		const float zCosine{ CatalystBaseMath::CosineRadians(zRadians) };
		const float zSine{ CatalystBaseMath::SineRadians(zRadians) };

		//Rotate the roll.
		float tempY = _Y * xCosine - _Z * xSine;
		_Z = _Y * xSine + _Z * xCosine;
		_Y = tempY;

		//Rotate the pitch
		float tempX1 = _X * yCosine + _Z * ySine;
		_Z = -_X * ySine + _Z * yCosine;
		_X = tempX1;

		//Rotate the yaw.
		float tempX2 = _X * zCosine - _Y * zSine;
		_Y = _X * zSine + _Y * zCosine;
		_X = tempX2;
	}

	/*
	*	Returns a rotate copy of this vector.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 Rotated(const Vector3& rotationVector) const NOEXCEPT
	{
		Vector3 copy{ *this };
		copy.Rotate(rotationVector);

		return copy;
	}

};

//Inline functions.
#include <Math/Vector3.inl>