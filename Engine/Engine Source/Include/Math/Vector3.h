#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
	*	Returns the dot product of two vectors.
	*/
	static float DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT;

	/*
	*	Returns the cross product of two vectors.
	*/
	static Vector3 CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT;

	/*
	*	Given a vector, return it's length squared.
	*/
	static float LengthSquared(const Vector3 &vector) NOEXCEPT;

	/*
	*	Given a vector, return it's unit vector counterpart.
	*/
	static Vector3 Normalize(const Vector3 &vector) NOEXCEPT;

	/*
	*	Default constructor.
	*/
	constexpr Vector3() NOEXCEPT
		:
		X(0.0f),
		Y(0.0f),
		Z(0.0f)
	{

	}

	/*
	*	Constructor taking a single scalar, applying it to both X, Y and Z.
	*/
	constexpr Vector3(const float scalar) NOEXCEPT
		:
		X(scalar),
		Y(scalar),
		Z(scalar)
	{

	}

	/*
	*	Constructor taking X, Y and Z as arguments.
	*/
	constexpr Vector3(const float newX, const float newY, const float newZ) NOEXCEPT
		:
		X(newX),
		Y(newY),
		Z(newZ)
	{

	}

	/*
	*	Copy constructor taking another vector as argument.
	*/
	constexpr Vector3(const Vector3 &otherVector) NOEXCEPT
		:
		X(otherVector.X),
		Y(otherVector.Y),
		Z(otherVector.Z)
	{

	}

	/*
	*	Assignment operator overload.
	*/
	void operator=(const Vector3 &otherVector) NOEXCEPT
	{
		X = otherVector.X;
		Y = otherVector.Y;
		Z = otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 addition operator overload.
	*/
	Vector3 operator+(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ X + otherVector.X, Y + otherVector.Y, Z + otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 addition assignment operator overload.
	*/
	void operator+=(const Vector3 &otherVector) NOEXCEPT
	{
		X += otherVector.X;
		Y += otherVector.Y;
		Z += otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 subtraction operator overload.
	*/
	Vector3 operator-(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ X - otherVector.X, Y - otherVector.Y, Z - otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 subtraction assignment operator overload.
	*/
	void operator-=(const Vector3 &otherVector) NOEXCEPT
	{
		X -= otherVector.X;
		Y -= otherVector.Y;
		Z -= otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 multiplication operator overload.
	*/
	Vector3 operator*(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ X * otherVector.X, Y * otherVector.Y, Z * otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 multiplication assignment operator overload.
	*/
	void operator*=(const Vector3 &otherVector) NOEXCEPT
	{
		X *= otherVector.X;
		Y *= otherVector.Y;
		Z *= otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 division operator overload.
	*/
	Vector3 operator/(const Vector3 &otherVector) const NOEXCEPT
	{
		return Vector3{ X / otherVector.X, Y / otherVector.Y, Z / otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 division assignment operator overload.
	*/
	void operator/=(const Vector3 &otherVector) NOEXCEPT
	{
		X /= otherVector.X;
		Y /= otherVector.Y;
		Z /= otherVector.Z;
	}

	/*
	*	Vector3 by scalar addition operator overload.
	*/
	Vector3 operator+(const float scalar) const NOEXCEPT
	{
		return Vector3{ X + scalar, Y + scalar, Z + scalar };
	}

	/*
	*	Vector3 by scalar addition assignment operator overload.
	*/
	void operator+=(const float scalar) NOEXCEPT
	{
		X += scalar;
		Y += scalar;
		Z += scalar;
	}

	/*
	*	Vector3 by scalar subtraction operator overload.
	*/
	Vector3 operator-(const float scalar) const NOEXCEPT
	{
		return Vector3{ X - scalar, Y - scalar, Z - scalar };
	}

	/*
	*	Vector3 by scalar subtraction assignment operator overload.
	*/
	void operator-=(const float scalar) NOEXCEPT
	{
		X -= scalar;
		Y -= scalar;
		Z -= scalar;
	}

	/*
	*	Vector3 by scalar multiplication operator overload.
	*/
	Vector3 operator*(const float scalar) const NOEXCEPT
	{
		return Vector3{ X * scalar, Y * scalar, Z * scalar };
	}

	/*
	*	Vector3 by scalar multiplication assignment operator overload.
	*/
	void operator*=(const float scalar) NOEXCEPT
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
	}

	/*
	*	Vector3 by scalar division operator overload.
	*/
	Vector3 operator/(const float scalar) const NOEXCEPT
	{
		return Vector3{ X / scalar, Y / scalar, Z / scalar };
	}

	/*
	*	Vector3 by scalar division assignment operator overload.
	*/
	void operator/=(const float scalar) NOEXCEPT
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
	}

	/*
	*	Returns a pointer to this vector's data, const.
	*/
	RESTRICTED const float* const Data() const NOEXCEPT { return &X; }

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	RESTRICTED float* Data() NOEXCEPT { return &X; }

	/*
	*	Returns the length of the vector.
	*/
	float Length() const NOEXCEPT;

	/*
	*	Returns the length of the vector squared.
	*/
	float LengthSquared() const  NOEXCEPT { return (X * X) + (Y * Y) + (Z * Z); }

	/*
	*	Returns the distance between this and another vector.
	*/
	float DistanceTo(const Vector3 &otherVector) const  NOEXCEPT { return (*this - otherVector).Length(); }

	/*
	*	Returns the distance squared between this and another vector.
	*/
	float DistanceToSquared(const Vector3 &otherVector) const  NOEXCEPT { return (*this - otherVector).LengthSquared(); }

	/*
	*	Returns true if the vector is a unit vector, false otherwise.
	*/
	bool IsUnitVector() const  NOEXCEPT { return LengthSquared() == 1.0f; }

	/*
	*	Normalize the vector to a unit vector.
	*/
	void Normalize() NOEXCEPT;

	/*
	*	Given a rotation vector, rotate this vector.
	*/
	void Rotate(const Vector3& rotationVector) NOEXCEPT;

	/*
	*	Given a rotation vector, rotate this vector.
	*/
	void NewRotate(const Vector3& rotationVector) NOEXCEPT;

	/*
	*	Returns a rotate copy of this vector.
	*/
	Vector3 Rotated(const Vector3& rotationVector) const NOEXCEPT;

};