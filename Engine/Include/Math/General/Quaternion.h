#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/EulerAngles.h>

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
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Quaternion() NOEXCEPT
		:
		_X(0.0f),
		_Y(0.0f),
		_Z(0.0f),
		_W(1.0f)
	{

	}

	/*
	*	Constructor taking all the values as arguments.
	*/
	FORCE_INLINE constexpr Quaternion(const float32 initial_x, const float32 initial_y, const float32 initial_z, const float32 initial_w) NOEXCEPT
		:
		_X(initial_x),
		_Y(initial_y),
		_Z(initial_z),
		_W(initial_w)
	{

	}

	/*
	*	Constructor taking an axis and an angle.
	*/
	FORCE_INLINE constexpr Quaternion(const Vector3<float32> &axis, const float32 angle) NOEXCEPT
		:
		_X(axis._X * CatalystBaseMath::Sine(angle * 0.5f)),
		_Y(axis._Y * CatalystBaseMath::Sine(angle * 0.5f)),
		_Z(axis._Z * CatalystBaseMath::Sine(angle * 0.5f)),
		_W(CatalystBaseMath::Cosine(angle * 0.5f))
	{

	}

	/*
	*	Quaternion by quaternion multiplication operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Quaternion operator*(const Quaternion &other) const NOEXCEPT
	{
		return Quaternion(	(_W * other._X) + (_X * other._W) + (_Y * other._Z) - (_Z * other._Y),
							(_W * other._Y) - (_X * other._Z) + (_Y * other._W) + (_Z * other._X),
							(_W * other._Z) + (_X * other._Y) - (_Y * other._X) + (_Z * other._W),
							(_W * other._W) - (_X * other._X) -( _Y * other._Y) - (_Z * other._Z));
	}

	/*
	*	Returns if this quaternion is normalized or not (is a unit quaternion).
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsNormalized() const NOEXCEPT
	{
		return MagnitudeSquared() == 1.0f;
	}

	/*
	*	Returns the magnitude of this quaternion.
	*/
	FORCE_INLINE constexpr NO_DISCARD float32 Magnitude() const NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(MagnitudeSquared());
	}

	/*
	*	Returns the squared magnitude of this quaternion.
	*/
	FORCE_INLINE constexpr NO_DISCARD float32 MagnitudeSquared() const NOEXCEPT
	{
		return (_X * _X) + (_Y * _Y) + (_Z * _Z) + (_W * _W);
	}

	/*
	*	Normalizes this quaternion.
	*/
	FORCE_INLINE constexpr void Normalize() NOEXCEPT
	{
		const float32 magnitude{ Magnitude() };

		const float32 inverse_magnitude{ 1.0f / magnitude };

		_X *= inverse_magnitude;
		_Y *= inverse_magnitude;
		_Z *= inverse_magnitude;
		_W *= inverse_magnitude;
	}

	/*
	*	Converts this quaternion to euler angles.
	*/
	FORCE_INLINE NO_DISCARD EulerAngles ToEulerAngles() const NOEXCEPT
	{
		EulerAngles angles;

		//Roll.
		{
			const float32 y{ 2.0f * (_Y * _Z + _W * _X) };
			const float32 x{ _W * _W - _X * _X - _Y * _Y + _Z * _Z };

			if (CatalystBaseMath::Absolute(x) <= FLOAT32_EPSILON
				&& CatalystBaseMath::Absolute(y) <= FLOAT32_EPSILON)
			{
				angles._Roll = 2.0f * CatalystBaseMath::ArcTangent(_X, _W);
			}

			else
			{
				angles._Roll = CatalystBaseMath::ArcTangent(y, x);
			}
		}

		//Yaw.
		{
			angles._Yaw = CatalystBaseMath::ArcSine(CatalystBaseMath::Clamp(-2.0f * (_X * _Z - _W * _Y), -1.0f, 1.0f));
		}

		//Pitch.
		{
			const float32 y{ 2.0f * (_X * _Y + _W * _Z) };
			const float32 x{ _W * _W + _X * _X - _Y * _Y - _Z * _Z };

			if (CatalystBaseMath::Absolute(x) <= FLOAT32_EPSILON
				&& CatalystBaseMath::Absolute(y) <= FLOAT32_EPSILON)
			{
				angles._Pitch = 0.0f;
			}

			else
			{
				angles._Pitch = CatalystBaseMath::ArcTangent(y, x);
			}
		}

		return angles;
	}

	/*
	*	Converts euler angles to this quaternion.
	*/
	FORCE_INLINE void FromEulerAngles(const EulerAngles &angles) NOEXCEPT
	{
		*this = Quaternion(Vector3<float32>(1.0f, 0.0f, 0.0f), angles._Roll) * Quaternion(Vector3<float32>(0.0f, 1.0f, 0.0f), angles._Yaw) * Quaternion(Vector3<float32>(0.0f, 0.0f, 1.0f), angles._Pitch);
	}

};