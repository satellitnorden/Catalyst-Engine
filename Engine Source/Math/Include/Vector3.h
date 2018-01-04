#pragma once

//Engine core.
#include <EngineCore.h>

class Vector3
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
	static float DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) CATALYST_NOEXCEPT;

	/*
	*	Returns the cross product of two vectors.
	*/
	static Vector3 CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) CATALYST_NOEXCEPT;

	/*
	*	Given a vector, return it's length squared.
	*/
	static float LengthSquared(const Vector3 &vector) CATALYST_NOEXCEPT;

	/*
	*	Given a vector, return it's unit vector counterpart.
	*/
	static Vector3 Normalize(const Vector3 &vector) CATALYST_NOEXCEPT;

	/*
	*	Default constructor.
	*/
	Vector3() CATALYST_NOEXCEPT;

	/*
	*	Constructor taking X, Y and Z as arguments.
	*/
	Vector3(const float newX, const float newY, const float newZ) CATALYST_NOEXCEPT;

	/*
	*	Copy constructor taking another vector as argument.
	*/
	Vector3(const Vector3 &otherVector) CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~Vector3() CATALYST_NOEXCEPT;

	/*
	*	Assignment operator overload.
	*/
	void operator=(const Vector3 &otherVector) CATALYST_NOEXCEPT
	{
		X = otherVector.X;
		Y = otherVector.Y;
		Z = otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 addition operator overload.
	*/
	Vector3 operator+(const Vector3 &otherVector) const CATALYST_NOEXCEPT
	{
		return Vector3{ X + otherVector.X, Y + otherVector.Y, Z + otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 addition assignment operator overload.
	*/
	void operator+=(const Vector3 &otherVector) CATALYST_NOEXCEPT
	{
		X += otherVector.X;
		Y += otherVector.Y;
		Z += otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 subtraction operator overload.
	*/
	Vector3 operator-(const Vector3 &otherVector) const CATALYST_NOEXCEPT
	{
		return Vector3{ X - otherVector.X, Y - otherVector.Y, Z - otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 subtraction assignment operator overload.
	*/
	void operator-=(const Vector3 &otherVector) CATALYST_NOEXCEPT
	{
		X -= otherVector.X;
		Y -= otherVector.Y;
		Z -= otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 multiplication operator overload.
	*/
	Vector3 operator*(const Vector3 &otherVector) const CATALYST_NOEXCEPT
	{
		return Vector3{ X * otherVector.X, Y * otherVector.Y, Z * otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 multiplication assignment operator overload.
	*/
	void operator*=(const Vector3 &otherVector) CATALYST_NOEXCEPT
	{
		X *= otherVector.X;
		Y *= otherVector.Y;
		Z *= otherVector.Z;
	}

	/*
	*	Vector3 by Vector3 division operator overload.
	*/
	Vector3 operator/(const Vector3 &otherVector) const CATALYST_NOEXCEPT
	{
		return Vector3{ X / otherVector.X, Y / otherVector.Y, Z / otherVector.Z };
	}

	/*
	*	Vector3 by Vector3 division assignment operator overload.
	*/
	void operator/=(const Vector3 &otherVector) CATALYST_NOEXCEPT
	{
		X /= otherVector.X;
		Y /= otherVector.Y;
		Z /= otherVector.Z;
	}

	/*
	*	Vector3 by scalar addition operator overload.
	*/
	Vector3 operator+(const float scalar) const CATALYST_NOEXCEPT
	{
		return Vector3{ X + scalar, Y + scalar, Z + scalar };
	}

	/*
	*	Vector3 by scalar addition assignment operator overload.
	*/
	void operator+=(const float scalar) CATALYST_NOEXCEPT
	{
		X += scalar;
		Y += scalar;
		Z += scalar;
	}

	/*
	*	Vector3 by scalar subtraction operator overload.
	*/
	Vector3 operator-(const float scalar) const CATALYST_NOEXCEPT
	{
		return Vector3{ X - scalar, Y - scalar, Z - scalar };
	}

	/*
	*	Vector3 by scalar subtraction assignment operator overload.
	*/
	void operator-=(const float scalar) CATALYST_NOEXCEPT
	{
		X -= scalar;
		Y -= scalar;
		Z -= scalar;
	}

	/*
	*	Vector3 by scalar multiplication operator overload.
	*/
	Vector3 operator*(const float scalar) const CATALYST_NOEXCEPT
	{
		return Vector3{ X * scalar, Y * scalar, Z * scalar };
	}

	/*
	*	Vector3 by scalar multiplication assignment operator overload.
	*/
	void operator*=(const float scalar) CATALYST_NOEXCEPT
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
	}

	/*
	*	Vector3 by scalar division operator overload.
	*/
	Vector3 operator/(const float scalar) const CATALYST_NOEXCEPT
	{
		return Vector3{ X / scalar, Y / scalar, Z / scalar };
	}

	/*
	*	Vector3 by scalar division assignment operator overload.
	*/
	void operator/=(const float scalar) CATALYST_NOEXCEPT
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
	}

	/*
	*	Returns a pointer to this vector's data, const.
	*/
	CATALYST_RESTRICTED const float* const Data() const CATALYST_NOEXCEPT { return &X; }

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	CATALYST_RESTRICTED float* Data() CATALYST_NOEXCEPT { return &X; }

	/*
	*	Returns the length of the vector.
	*/
	float Length() const CATALYST_NOEXCEPT;

	/*
	*	Returns the length of the vector squared.
	*/
	float LengthSquared() const  CATALYST_NOEXCEPT { return (X * X) + (Y * Y) + (Z * Z); }

	/*
	*	Returns the distance between this and another vector.
	*/
	float DistanceTo(const Vector3 &otherVector) const  CATALYST_NOEXCEPT { return (*this - otherVector).Length(); }

	/*
	*	Returns the distance squared between this and another vector.
	*/
	float DistanceToSquared(const Vector3 &otherVector) const  CATALYST_NOEXCEPT { return (*this - otherVector).LengthSquared(); }

	/*
	*	Returns true if the vector is a unit vector, false otherwise.
	*/
	bool IsUnitVector() const  CATALYST_NOEXCEPT { return LengthSquared() == 1.0f; }

	/*
	*	Normalize the vector to a unit vector.
	*/
	void Normalize() CATALYST_NOEXCEPT;

	/*
	*	Given a rotation vector, rotate this vector.
	*/
	void Rotate(const Vector3& rotationVector) CATALYST_NOEXCEPT;

	/*
	*	Given a rotation vector, rotate this vector.
	*/
	void NewRotate(const Vector3& rotationVector) CATALYST_NOEXCEPT;

	/*
	*	Returns a rotate copy of this vector.
	*/
	Vector3 Rotated(const Vector3& rotationVector) const CATALYST_NOEXCEPT;

};