#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

/*
*	Vector2 definition.
*	Defines a vector with two components of the specified type.
*/
template <typename TYPE>
class Vector2 final
{

public:

	union
	{
		struct
		{
			//The X component.
			TYPE _X;

			//The Y component.
			TYPE _Y;
		};

		struct
		{
			//The R component.
			TYPE _R;

			//The G component.
			TYPE _G;
		};

		//The data.
		TYPE _Data[2];
	};

	/*
	*	Given a vector, clamp it between the minimum and the maximum values
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Clamp(const Vector2& vector, const float minimum, const float maximum) NOEXCEPT
	{
		return Vector2(CatalystBaseMath::Clamp<TYPE>(vector._X, minimum, maximum), CatalystBaseMath::Clamp<TYPE>(vector._Y, minimum, maximum));
	}

	/*
	*	Returns the dot product of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float DotProduct(const Vector2 &first, const Vector2 &second) NOEXCEPT
	{
		return first._X * second._X + first._Y * second._Y;
	}

	/*
	*	Given a vector, return it's length.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Length(const Vector2 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquared(vector));
	}

	/*
	*	Given a vector, return it's length squared.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthSquared(const Vector2 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Y * vector._Y;
	}

	/*
	*	Returns the minimum of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector2 Minimum(const Vector2 &first, const Vector2 &second) NOEXCEPT
	{
		return Vector2(	CatalystBaseMath::Minimum<TYPE>(first._X, second._X),
						CatalystBaseMath::Minimum<TYPE>(first._Y, second._Y));
	}

	/*
	*	Returns the maximum of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector2 Maximum(const Vector2 &first, const Vector2 &second) NOEXCEPT
	{
		return Vector2(	CatalystBaseMath::Maximum<TYPE>(first._X, second._X),
						CatalystBaseMath::Maximum<TYPE>(first._Y, second._Y));
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Vector2() NOEXCEPT
		:
		_X(static_cast<TYPE>(0)),
		_Y(static_cast<TYPE>(0))
	{

	}

	/*
	*	Constructor taking X and Y arguments.
	*/
	FORCE_INLINE constexpr Vector2(const TYPE initialX, const TYPE initialY) NOEXCEPT
		:
		_X(initialX),
		_Y(initialY)
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE TYPE operator[](const uint64 index) const NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE TYPE& operator[](const uint64 index)  NOEXCEPT
	{
		return _Data[index];
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
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE* const RESTRICT Data() const NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE* RESTRICT Data() NOEXCEPT
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
		return _X * _X + _Y * _Y;
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
	*	Given an amount, rotate the vector.
	*/
	FORCE_INLINE constexpr void Rotate(const float amount) NOEXCEPT
	{
		const float cosine{ CatalystBaseMath::Cosine(amount) };
		const float sine{ CatalystBaseMath::Sine(amount) };

		const float newX{ _X * cosine - _Y * sine };
		const float newY{ _X * sine + _Y * cosine };

		_X = newX;
		_Y = newY;
	}

};

/*
*	Vector3 definition.
*	Defines a vector with three components of the specified type.
*/
template <typename TYPE>
class Vector3 final
{

public:

	union
	{
		struct
		{
			//The X component.
			TYPE _X;

			//The Y component.
			TYPE _Y;

			//The Z component.
			TYPE _Z;
		};

		struct
		{
			//The R component.
			TYPE _R;

			//The G component.
			TYPE _G;

			//The B component.
			TYPE _B;
		};

		//The data.
		TYPE _Data[3];
	};

	/*
	*	Returns the absolute value of the Vector3 passed in.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 Absolute(const Vector3 &vector) NOEXCEPT
	{
		return Vector3(CatalystBaseMath::Absolute(vector._X), CatalystBaseMath::Absolute(vector._Y), CatalystBaseMath::Absolute(vector._Z));
	}

	/*
	*	Returns the dot product of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
	{
		return firstVector._X * secondVector._X + firstVector._Y * secondVector._Y + firstVector._Z * secondVector._Z;
	}

	/*
	*	Returns the cross product of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
	{
		return Vector3{ firstVector._Y * secondVector._Z - firstVector._Z * secondVector._Y, firstVector._Z * secondVector._X - firstVector._X * secondVector._Z, firstVector._X * secondVector._Y - firstVector._Y * secondVector._X };
	}

	/*
	*	Makes a direction point in the same hemisphere as the given normal.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 Hemisphere(const Vector3 &direction, const Vector3 &normal) NOEXCEPT
	{
		return DotProduct(direction, normal) >= 0.0f ? direction : -direction;
	}

	/*
	*	Returns whether point A is closer to the given position than B or not.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool IsCloser(const Vector3<float> &position, const Vector3<float> &A, const Vector3<float> &B) NOEXCEPT
	{
		return Vector3<float>::LengthSquared(position - A) < Vector3<float>::LengthSquared(position - B);
	}

	/*
	*	Given a vector, return it's length.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Length(const Vector3 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquared(vector));
	}

	/*
	*	Given a vector, return it's length, only taking into account the X and Y axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthXY(const Vector3 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquaredXY(vector));
	}

	/*
	*	Given a vector, return it's length, only taking into account the X and Z axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthXZ(const Vector3 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquaredXZ(vector));
	}

	/*
	*	Given a vector, return it's length, only taking into account the X and Y axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthYZ(const Vector3 &vector) NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(LengthSquaredYZ(vector));
	}

	/*
	*	Given a vector, return it's length squared.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthSquared(const Vector3 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Y * vector._Y + vector._Z * vector._Z;
	}

	/*
	*	Given a vector, return it's length squared, only taking into account the X and Y axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthSquaredXY(const Vector3 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Y * vector._Y;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthSquaredXZ(const Vector3 &vector) NOEXCEPT
	{
		return vector._X * vector._X + vector._Z * vector._Z;
	}

	/*
	*	Given a vector, return it's length squared only taking into account the X and Y axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LengthSquaredYZ(const Vector3 &vector) NOEXCEPT
	{
		return vector._Y * vector._Y + vector._Z * vector._Z;
	}

	/*
	*	Linearly interpolate between two float vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 LinearlyInterpolate(const Vector3 &first, const Vector3 &second, const float alpha) NOEXCEPT
	{
		return Vector3(CatalystBaseMath::LinearlyInterpolate(first._X, second._X, alpha), CatalystBaseMath::LinearlyInterpolate(first._Y, second._Y, alpha), CatalystBaseMath::LinearlyInterpolate(first._Z, second._Z, alpha));
	}

	/*
	*	Returns the minimum of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 Minimum(const Vector3 &first, const Vector3 &second) NOEXCEPT
	{
		return Vector3(	CatalystBaseMath::Minimum<TYPE>(first._X, second._X),
						CatalystBaseMath::Minimum<TYPE>(first._Y, second._Y),
						CatalystBaseMath::Minimum<TYPE>(first._Z, second._Z));
	}

	/*
	*	Returns the maximum of two vectors.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 Maximum(const Vector3 &first, const Vector3 &second) NOEXCEPT
	{
		return Vector3(	CatalystBaseMath::Maximum<TYPE>(first._X, second._X),
						CatalystBaseMath::Maximum<TYPE>(first._Y, second._Y),
						CatalystBaseMath::Maximum<TYPE>(first._Z, second._Z));
	}

	/*
	*	Given a vector, return it's unit vector counterpart.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 Normalize(const Vector3 &vector) NOEXCEPT
	{
		return vector.Normalized();
	}

	/*
	*	Given a vector, return it's unit vector counterpart on the XY axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 NormalizeXY(const Vector3 &vector) NOEXCEPT
	{
		return Vector3<float>(vector._X, vector._Y, 0.0f).Normalized();
	}

	/*
	*	Given a vector, return it's unit vector counterpart on the XY axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 NormalizeXZ(const Vector3 &vector) NOEXCEPT
	{
		return Vector3<float>(vector._X, 0.0f, vector._Z).Normalized();
	}

	/*
	*	Given a vector, return it's unit vector counterpart on the XY axis.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 NormalizeYZ(const Vector3 &vector) NOEXCEPT
	{
		return Vector3<float>(0.0f, vector._Y, vector._Z).Normalized();
	}

	/*
	*	Reflects the incoming direction onto the given normal.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3 Reflect(const Vector3 &incoming, const Vector3 &normal) NOEXCEPT
	{
		return incoming - 2.0f * Vector3<float>::DotProduct(incoming, normal) * normal;
	}

	/*
	*	Given a rotation vector, returns the forward vector.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> ForwardVector(const Vector3<float> &rotation) NOEXCEPT
	{
		return Vector3<float>::Normalize(VectorConstants::FORWARD.Rotated(rotation));
	}

	/*
	*	Given a rotation vector, returns the right vector.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> RightVector(const Vector3<float> &rotation) NOEXCEPT
	{
		return Vector3<float>::Normalize(VectorConstants::RIGHT.Rotated(rotation));
	}

	/*
	*	Given a rotation vector, returns the up vector.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> UpVector(const Vector3<float> &rotation) NOEXCEPT
	{
		return Vector3<float>::Normalize(VectorConstants::UP.Rotated(rotation));
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Vector3() NOEXCEPT
		:
		_X(static_cast<TYPE>(0)),
		_Y(static_cast<TYPE>(0)),
		_Z(static_cast<TYPE>(0))
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
	FORCE_INLINE constexpr Vector3(const TYPE initialX, const TYPE initialY, const TYPE initialZ) NOEXCEPT
		:
		_X(initialX),
		_Y(initialY),
		_Z(initialZ)
	{

	}

	/*
	*	Constructor taking a Vector2 and a float.
	*/
	FORCE_INLINE constexpr Vector3(const Vector2<float> &initialXY, const float initialZ) NOEXCEPT
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
	*	Equality operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool operator==(const Vector3 &other) const NOEXCEPT
	{
		return	_X == other._X
			&& _Y == other._Y
			&& _Z == other._Z;
	}

	/*
	*	Inquality operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool operator!=(const Vector3 &other) NOEXCEPT
	{
		return !(*this == other);
	}

	/*
	*	Negative operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator-() const NOEXCEPT
	{
		return Vector3<float>(-_X, -_Y, -_Z);
	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE constexpr NO_DISCARD TYPE operator[](const uint64 index) const NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE constexpr NO_DISCARD TYPE& operator[](const uint64 index)  NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Vector3 by scalar addition operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator+(const float scalar) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator-(const float scalar) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator*(const float scalar) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator/(const float scalar) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator+(const Vector3 &otherVector) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator-(const Vector3 &otherVector) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator*(const Vector3 &otherVector) const NOEXCEPT
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
	FORCE_INLINE constexpr NO_DISCARD Vector3 operator/(const Vector3 &otherVector) const NOEXCEPT
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
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE* const RESTRICT Data() const NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns a pointer to this vector's data, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE* RESTRICT Data() NOEXCEPT
	{
		return &_X;
	}

	/*
	*	Returns true if the vector is a unit vector, false otherwise.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsUnitVector() const NOEXCEPT
	{
		return LengthSquared() == 1.0f;
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
	*	Normalize the vector to a unit vector, ignoring the Z axis.
	*/
	FORCE_INLINE constexpr void NormalizeXY() NOEXCEPT
	{
		_Z = 0.0f;

		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		_X *= inverseLength;
		_Y *= inverseLength;
	}

	/*
	*	Normalize the vector to a unit vector, ignoring the Y axis.
	*/
	FORCE_INLINE constexpr void NormalizeXZ() NOEXCEPT
	{
		_Y = 0.0f;

		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		_X *= inverseLength;
		_Z *= inverseLength;
	}

	/*
	*	Normalize the vector to a unit vector, ignoring the X axis.
	*/
	FORCE_INLINE constexpr void NormalizeYZ() NOEXCEPT
	{
		_X = 0.0f;

		const float length{ Length() };
		const float inverseLength{ 1.0f / length };

		_Y *= inverseLength;
		_Z *= inverseLength;
	}

	/*
	*	Returns a normalized version of this vector.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3 Normalized() const NOEXCEPT
	{
		Vector3 copy{ *this };
		copy.Normalize();

		return copy;
	}

	/*
	*	Returns whether or not this vector is normalized.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsNormalized() const NOEXCEPT
	{
		return LengthSquared() == 1.0f;
	}

	/*
	*	Given a rotation vector, rotate this vector.
	*/
	FORCE_INLINE constexpr void Rotate(const Vector3& rotationVector) NOEXCEPT
	{
		const float xCosine{ CatalystBaseMath::Cosine(rotationVector._X) };
		const float xSine{ CatalystBaseMath::Sine(rotationVector._X) };

		const float yCosine{ CatalystBaseMath::Cosine(rotationVector._Y) };
		const float ySine{ CatalystBaseMath::Sine(rotationVector._Y) };

		const float zCosine{ CatalystBaseMath::Cosine(rotationVector._Z) };
		const float zSine{ CatalystBaseMath::Sine(rotationVector._Z) };

		//Rotate the roll.
		if (rotationVector._X != 0.0f)
		{
			float tempY = _Y * xCosine - _Z * xSine;
			_Z = _Y * xSine + _Z * xCosine;
			_Y = tempY;
		}

		//Rotate the pitch
		if (rotationVector._Y != 0.0f)
		{
			float tempX1 = _X * yCosine + _Z * ySine;
			_Z = -_X * ySine + _Z * yCosine;
			_X = tempX1;
		}

		//Rotate the yaw.
		if (rotationVector._Z != 0.0f)
		{
			float tempX2 = _X * zCosine - _Y * zSine;
			_Y = _X * zSine + _Y * zCosine;
			_X = tempX2;
		}
	}

	/*
	*	Returns a rotate copy of this vector.
	*/
	FORCE_INLINE constexpr NO_DISCARD Vector3 Rotated(const Vector3& rotationVector) const NOEXCEPT
	{
		Vector3 copy{ *this };
		copy.Rotate(rotationVector);

		return copy;
	}

};

/*
*	Vector4 definition.
*	Defines a vector with four components of the specified type.
*/
template <typename TYPE>
class Vector4 final
{

public:

	union
	{
		struct
		{
			//The X component.
			TYPE _X;

			//The Y component.
			TYPE _Y;

			//The Z component.
			TYPE _Z;

			//The W component.
			TYPE _W;
		};

		struct
		{
			//The R component.
			TYPE _R;

			//The G component.
			TYPE _G;

			//The B component.
			TYPE _B;

			//The A component.
			TYPE _A;
		};

		//The data.
		TYPE _Data[4];

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
	FORCE_INLINE constexpr Vector4() NOEXCEPT
		:
		_X(static_cast<TYPE>(0)),
		_Y(static_cast<TYPE>(0)),
		_Z(static_cast<TYPE>(0)),
		_W(static_cast<TYPE>(0))
	{

	}

	/*
	*	Constructor taking a single float as argument.
	*/
	FORCE_INLINE constexpr Vector4(const float newValue) NOEXCEPT
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
	FORCE_INLINE constexpr Vector4(const TYPE initialX, const TYPE initialY, const TYPE initialZ, const TYPE initialW) NOEXCEPT
		:
		_X(initialX),
		_Y(initialY),
		_Z(initialZ),
		_W(initialW)
	{

	}

	/*
	*	Constructor taking a Vector3 as argument.
	*/
	FORCE_INLINE constexpr Vector4(const Vector3<float> &otherVector) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z),
		_W(static_cast<TYPE>(1))
	{

	}

	/*
	*	Constructor taking a Vector3 and a scalar as argument.
	*/
	FORCE_INLINE constexpr Vector4(const Vector3<float> &otherVector, const float scalar) NOEXCEPT
		:
		_X(otherVector._X),
		_Y(otherVector._Y),
		_Z(otherVector._Z),
		_W(scalar)
	{

	}

	/*
	*	Equality operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool operator==(const Vector4 &other) const NOEXCEPT
	{
		return	_X == other._X
				&& _Y == other._Y
				&& _Z == other._Z
				&& _W == other._W;
	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE TYPE operator[](const uint64 index) const NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE TYPE& operator[](const uint64 index)  NOEXCEPT
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
		return Vector4{ static_cast<TYPE>(static_cast<float>(_X) * scalar),
						static_cast<TYPE>(static_cast<float>(_Y) * scalar),
						static_cast<TYPE>(static_cast<float>(_Z) * scalar),
						static_cast<TYPE>(static_cast<float>(_W) * scalar) };
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
		return Vector4{ static_cast<TYPE>(_X + otherVector._X), static_cast<TYPE>(_Y + otherVector._Y), static_cast<TYPE>(_Z + otherVector._Z), static_cast<TYPE>(_W + otherVector._W) };
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

//Vector constants.
namespace VectorConstants
{
	constexpr Vector3<float> FORWARD{ 0.0f, 0.0f, -1.0f };
	constexpr Vector3<float> BACKWARD{ 0.0f, 0.0f, 1.0f };
	constexpr Vector3<float> UP{ 0.0f, 1.0f, 0.0f };
	constexpr Vector3<float> DOWN{ 0.0f, -1.0f, 0.0f };
	constexpr Vector3<float> RIGHT{ 1.0f, 0.0f, 0.0f };
	constexpr Vector3<float> LEFT{ -1.0f, 0.0f, 0.0f };
	constexpr Vector3<float> ZERO{ 0.0f, 0.0f, 0.0f };
	constexpr Vector3<float> ONE{ 1.0f, 1.0f, 1.0f };
}

//Inline functions.
#include <Math/General/Vector.inl>