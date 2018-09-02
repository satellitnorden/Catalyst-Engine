#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystMath.h>

class Vector3 final
{

public:

	//The X component.
	float X;

	//The Y component.
	float Y;

	//The Z component.
	float Z;

	/*
	*	Returns the absolute value of the Vector3 passed in.
	*/
	FORCE_INLINE NO_DISCARD constexpr static Vector3 Absolute(const Vector3 &vector) NOEXCEPT
	{
		return Vector3(CatalystMath::Absolute(vector.X), CatalystMath::Absolute(vector.Y), CatalystMath::Absolute(vector.Z));
	}

	/*
	*	Returns the dot product of two vectors.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
	{
		return firstVector.X * secondVector.X + firstVector.Y * secondVector.Y + firstVector.Z * secondVector.Z;
	}

	/*
	*	Returns the cross product of two vectors.
	*/
	FORCE_INLINE NO_DISCARD constexpr static Vector3 CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
	{
		return Vector3{ firstVector.Y * secondVector.Z - firstVector.Z * secondVector.Y, firstVector.Z * secondVector.X - firstVector.X * secondVector.Z, firstVector.X * secondVector.Y - firstVector.Y * secondVector.X };
	}

	/*
	*	Given a vector, return it's length.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float Length(const Vector3 &vector) NOEXCEPT
	{
		return CatalystMath::SquareRoot(LengthSquared(vector));
	}

	/*
	*	Given a vector, return it's length squared.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquared(const Vector3 &vector) NOEXCEPT
	{
		return vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquaredXY(const Vector3 &vector) NOEXCEPT
	{
		return vector.X * vector.X + vector.Y * vector.Y;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquaredXZ(const Vector3 &vector) NOEXCEPT
	{
		return vector.X * vector.X + vector.Z * vector.Z;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE NO_DISCARD constexpr static float LengthSquaredYZ(const Vector3 &vector) NOEXCEPT
	{
		return vector.Y * vector.Y + vector.Z * vector.Z;
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
		X(0.0f),
		Y(0.0f),
		Z(0.0f)
	{

	}

	/*
	*	Constructor taking a single scalar, applying it to both X, Y and Z.
	*/
	FORCE_INLINE constexpr Vector3(const float scalar) NOEXCEPT
		:
		X(scalar),
		Y(scalar),
		Z(scalar)
	{

	}

	/*
	*	Constructor taking X, Y and Z as arguments.
	*/
	FORCE_INLINE constexpr Vector3(const float newX, const float newY, const float newZ) NOEXCEPT
		:
		X(newX),
		Y(newY),
		Z(newZ)
	{

	}

	/*
	*	Copy constructor taking another vector as argument.
	*/
	FORCE_INLINE constexpr Vector3(const Vector3 &otherVector) NOEXCEPT
		:
		X(otherVector.X),
		Y(otherVector.Y),
		Z(otherVector.Z)
	{

	}

	/*
	*	Assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const Vector3 &otherVector) NOEXCEPT
	{
		X = otherVector.X;
		Y = otherVector.Y;
		Z = otherVector.Z;
	}

	/*
	*	Vector3 by scalar addition operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator+(const float scalar) const NOEXCEPT
	{
		return Vector3{ X + scalar, Y + scalar, Z + scalar };
	}

	/*
	*	Vector3 by scalar addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const float scalar) NOEXCEPT
	{
		X += scalar;
		Y += scalar;
		Z += scalar;
	}

	/*
	*	Vector3 by scalar subtraction operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator-(const float scalar) const NOEXCEPT
	{
		return Vector3{ X - scalar, Y - scalar, Z - scalar };
	}

	/*
	*	Vector3 by scalar subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const float scalar) NOEXCEPT
	{
		X -= scalar;
		Y -= scalar;
		Z -= scalar;
	}

	/*
	*	Vector3 by scalar multiplication operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator*(const float scalar) const NOEXCEPT
	{
		return Vector3{ X * scalar, Y * scalar, Z * scalar };
	}

	/*
	*	Vector3 by scalar multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const float scalar) NOEXCEPT
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
	}

	/*
	*	Vector3 by scalar division operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator/(const float scalar) const NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		return Vector3{ X * inverseScalar, Y * inverseScalar, Z * inverseScalar };
	}

	/*
	*	Vector3 by scalar division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const float scalar) NOEXCEPT
	{
		const float inverseScalar{ 1.0f / scalar };

		X *= inverseScalar;
		Y *= inverseScalar;
		Z *= inverseScalar;
	}

	/*
	*	Vector3 by Vector3 addition operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator+(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ X + otherVector.X, Y + otherVector.Y, Z + otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 addition assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator+=(const Vector3 &otherVector) NOEXCEPT
	{
		X += otherVector.X;
		Y += otherVector.Y;
		Z += otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 subtraction operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator-(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ X - otherVector.X, this->Y - otherVector.Y, this->Z - otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 subtraction assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator-=(const Vector3 &otherVector) NOEXCEPT
	{
		X -= otherVector.X;
		Y -= otherVector.Y;
		Z -= otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 multiplication operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator*(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ this->X * otherVector.X, this->Y * otherVector.Y, this->Z * otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 multiplication assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator*=(const Vector3 &otherVector) NOEXCEPT
	{
		X *= otherVector.X;
		Y *= otherVector.Y;
		Z *= otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 division operator overload.
	*/
	FORCE_INLINE NO_DISCARD constexpr Vector3 operator/(const Vector3 &otherVector) const NOEXCEPT
	{ 
		return Vector3{ X / otherVector.X, Y / otherVector.Y, Z / otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 division assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator/=(const Vector3 &otherVector) NOEXCEPT
	{
		X /= otherVector.X;
		Y /= otherVector.Y;
		Z /= otherVector.Z;
	}

	/*
	*	Returns a pointer to this vector's data, const.
	*/
	FORCE_INLINE NO_DISCARD RESTRICTED constexpr const float* const RESTRICT Data() const NOEXCEPT
	{
		return &X;
	}

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	FORCE_INLINE NO_DISCARD RESTRICTED constexpr float* RESTRICT Data() NOEXCEPT
	{
		return &X;
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
		return CatalystMath::SquareRoot(LengthSquared());
	}

	/*
	*	Returns the length of the vector squared.
	*/
	FORCE_INLINE NO_DISCARD constexpr float LengthSquared() const NOEXCEPT
	{
		return (X * X) + (Y * Y) + (Z * Z);
	}

	/*
	*	Normalize the vector to a unit vector.
	*/
	FORCE_INLINE constexpr void Normalize() NOEXCEPT
	{
		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		X *= inverseLength;
		Y *= inverseLength;
		Z *= inverseLength;
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
		const float xRadians{ CatalystMath::DegreesToRadians(rotationVector.X) };
		const float yRadians{ CatalystMath::DegreesToRadians(rotationVector.Y) };
		const float zRadians{ CatalystMath::DegreesToRadians(rotationVector.Z) };

		const float xCosine{ CatalystMath::CosineRadians(xRadians) };
		const float xSine{ CatalystMath::SineRadians(xRadians) };

		const float yCosine{ CatalystMath::CosineRadians(yRadians) };
		const float ySine{ CatalystMath::SineRadians(yRadians) };

		const float zCosine{ CatalystMath::CosineRadians(zRadians) };
		const float zSine{ CatalystMath::SineRadians(zRadians) };

		//Rotate the roll.
		float tempY = Y * xCosine - Z * xSine;
		Z = Y * xSine + Z * xCosine;
		Y = tempY;

		//Rotate the pitch
		float tempX1 = X * yCosine + Z * ySine;
		Z = -X * ySine + Z * yCosine;
		X = tempX1;

		//Rotate the yaw.
		float tempX2 = X * zCosine - Y * zSine;
		Y = X * zSine + Y * zCosine;
		X = tempX2;
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