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
	*	Default constructor.
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
		const float32 sinr_cosp{ 2.0f * (_W * _X + _Y * _Z) };
		const float32 cosr_cosp{ 1.0f - 2.0f * (_X * _X + _Y * _Y) };
		angles._Roll = CatalystBaseMath::ArcTangent(sinr_cosp, cosr_cosp);

		//Pitch.
		const float32 sinp{ 2.0f * (_W * _Y - _Z * _X) };

		if (CatalystBaseMath::Absolute(sinp) >= 1.0f)
		{
			angles._Yaw = std::copysign(CatalystBaseMathConstants::HALF_PI, sinp);
		}

		else
		{
			angles._Yaw = std::asin(sinp);
		}

		//Yaw.
		const float32 siny_cosp{ 2.0f * (_W * _Z + _X * _Y) };
		const float32 cosy_cosp{ 1.0f - 2.0f * (_Y * _Y + _Z * _Z) };
		angles._Pitch = CatalystBaseMath::ArcTangent(siny_cosp, cosy_cosp);

		return angles;
	}

	/*
	*	Converts euler angles to this quaternion.
	*/
	FORCE_INLINE void FromEulerAngles(const EulerAngles &angles) NOEXCEPT
	{
		const float32 cr{ cos(angles._Roll * 0.5f) };
		const float32 sr{ sin(angles._Roll * 0.5f) };
		const float32 cp{ cos(angles._Yaw * 0.5f) };
		const float32 sp{ sin(angles._Yaw * 0.5f) };
		const float32 cy{ cos(angles._Pitch * 0.5f) };
		const float32 sy{ sin(angles._Pitch * 0.5f) };

		_X = sr * cp * cy - cr * sp * sy;
		_Y = cr * sp * cy + sr * cp * sy;
		_Z = cr * cp * sy - sr * sp * cy;
		_W = cr * cp * cy + sr * sp * sy;
	}

};