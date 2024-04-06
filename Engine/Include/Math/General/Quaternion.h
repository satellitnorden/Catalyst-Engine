#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>
#include <Math/General/Vector.h>

//Forward declarations.
class EulerAngles;

/*
*	Quaternion definition.
*	Defines a quaternion with four components.
*/
class Quaternion final
{

public:

	union
	{
		struct
		{
			//The X component.
			float32 _X;

			//The Y component.
			float32 _Y;

			//The Z component.
			float32 _Z;

			//The W component.
			float32 _W;
		};

		//The data.
		float32 _Data[4];

		struct
		{
			//The vector.
			Vector3<float32> _Vector;

			//The scalar.
			float32 _Scalar;
		};
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Quaternion() NOEXCEPT
		:
		_X(0.0f),
		_Y(0.0f),
		_Z(0.0f),
		_W(1.0f)
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE Quaternion(const Quaternion &other) NOEXCEPT
	{
		Memory::Copy(this, &other, sizeof(Quaternion));
	}

	/*
	*	Constructor taking all the values as arguments.
	*/
	FORCE_INLINE Quaternion(const float32 initial_x, const float32 initial_y, const float32 initial_z, const float32 initial_w) NOEXCEPT
		:
		_X(initial_x),
		_Y(initial_y),
		_Z(initial_z),
		_W(initial_w)
	{

	}

	/*
	*	Constructor taking a euler angles.
	*/
	FORCE_INLINE Quaternion(const EulerAngles &rotation) NOEXCEPT
	{
		FromEulerAngles(rotation);
	}

	/*
	*	Constructor taking an axis and an angle.
	*/
	FORCE_INLINE Quaternion(const Vector3<float32> &axis, const float32 angle) NOEXCEPT
		:
		_X(axis._X * BaseMath::Sine(angle * 0.5f)),
		_Y(axis._Y * BaseMath::Sine(angle * 0.5f)),
		_Z(axis._Z * BaseMath::Sine(angle * 0.5f)),
		_W(BaseMath::Cosine(angle * 0.5f))
	{

	}

	/*
	*	Assignment operator overload. constructor.
	*/
	FORCE_INLINE void operator=(const Quaternion &other) NOEXCEPT
	{
		Memory::Copy(this, &other, sizeof(Quaternion));
	}

	/*
	*	Quaternion by quaternion multiplication operator overload.
	*/
	FORCE_INLINE NO_DISCARD Quaternion operator*(const Quaternion &other) const NOEXCEPT
	{
		Quaternion result;

		result._Vector = other._Vector * _Scalar + _Vector * other._Scalar + Vector3<float32>::CrossProduct(_Vector, other._Vector);
		result._Scalar = _Scalar * other._Scalar - Vector3<float32>::DotProduct(_Vector, other._Vector);

		return result;
	}

	/*
	*	Returns if this quaternion is normalized or not (is a unit quaternion).
	*/
	FORCE_INLINE NO_DISCARD bool IsNormalized() const NOEXCEPT
	{
		return MagnitudeSquared() == 1.0f;
	}

	/*
	*	Returns the magnitude of this quaternion.
	*/
	FORCE_INLINE NO_DISCARD float32 Magnitude() const NOEXCEPT
	{
		return BaseMath::SquareRoot(MagnitudeSquared());
	}

	/*
	*	Returns the squared magnitude of this quaternion.
	*/
	FORCE_INLINE NO_DISCARD float32 MagnitudeSquared() const NOEXCEPT
	{
		return (_X * _X) + (_Y * _Y) + (_Z * _Z) + (_W * _W);
	}

	/*
	*	Normalizes this quaternion.
	*/
	FORCE_INLINE void Normalize() NOEXCEPT
	{
		const float32 inverse_magnitude{ BaseMath::InverseSquareRoot(MagnitudeSquared()) };

		_X *= inverse_magnitude;
		_Y *= inverse_magnitude;
		_Z *= inverse_magnitude;
		_W *= inverse_magnitude;
	}

	/*
	*	Converts this quaternion to euler angles.
	*/
	NO_DISCARD EulerAngles ToEulerAngles() const NOEXCEPT;

	/*
	*	Converts euler angles to this quaternion.
	*/
	void FromEulerAngles(const EulerAngles &angles) NOEXCEPT;

};