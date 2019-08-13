#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

/*
*	Quaternion definition.
*	Defines a quaternion with four components of the specified type.
*/
template <typename TYPE>
class Quaternion final
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

		//The data.
		TYPE _Data[4];
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Quaternion() NOEXCEPT
		:
		_X(static_cast<TYPE>(0)),
		_Y(static_cast<TYPE>(0)),
		_Z(static_cast<TYPE>(0)),
		_W(static_cast<TYPE>(0))
	{

	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Quaternion(const TYPE initial_x, const TYPE initial_y, const TYPE initial_z, const TYPE initial_w) NOEXCEPT
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
		return Quaternion(	_W * other._X + _X * other._W + _Y * other._Z - _Z * other._Y,
							_W * other._Y - _X * other._Z + _Y * other._W + _Z * other._X,
							_W * other._Z + _X * other._Y - _Y * other._X + _Z * other._W,
							_W * other._W - _X * other._X - _Y * other._Y - _Z * other._Z);
	}

	/*
	*	Returns the magnitude of this quaternion.
	*/
	FORCE_INLINE constexpr NO_DISCARD float Magnitude() const NOEXCEPT
	{
		return CatalystBaseMath::SquareRoot(MagnitudeSquared());
	}

	/*
	*	Returns the squared magnitude of this quaternion.
	*/
	FORCE_INLINE constexpr NO_DISCARD float MagnitudeSquared() const NOEXCEPT
	{
		return (_X * _X) + (_Y * _Y) + (_Z * _Z) + (_W * _W);
	}

	/*
	*	Normalizes this quaternion.
	*/
	FORCE_INLINE constexpr void Normalize() NOEXCEPT
	{
		const float magnitude{ Magnitude() };

		const float inverse_magnitude{ 1.0f / magnitude };

		_X *= inverse_magnitude;
		_Y *= inverse_magnitude;
		_Z *= inverse_magnitude;
		_W *= inverse_magnitude;
	}

};