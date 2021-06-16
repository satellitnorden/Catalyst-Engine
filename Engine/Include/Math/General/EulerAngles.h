#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Class representing Euler angles, with roll, yaw and pitch, all denoted in radians.
*/
class EulerAngles final
{

public:

	union
	{
		struct
		{
			/*
			*	The roll.
			*	Rotates counter-clockwise around the X axis.
			*/
			float32 _Roll;

			/*
			*	The yaw.
			*	Rotates counter-clockwise around the Y axis.
			*/
			float32 _Yaw;

			/*
			*	The pitch.
			*	Rotates counter-clockwise around the Z axis.
			*/
			float32 _Pitch;
		};

		//The data.
		float32 _Data[3];
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr EulerAngles() NOEXCEPT
		:
		_Roll(0.0f),
		_Yaw(0.0f),
		_Pitch(0.0f)
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE const float32 operator[](const uint64 index) const NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE float32 &operator[](const uint64 index) NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Constructor taking all the values as arguments.
	*/
	FORCE_INLINE constexpr EulerAngles(const float32 initial_roll, const float32 initial_yaw, const float32 initial_pitch) NOEXCEPT
		:
		_Roll(initial_roll),
		_Yaw(initial_yaw),
		_Pitch(initial_pitch)
	{

	}

	/*
	*	Negative operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD EulerAngles operator-() const NOEXCEPT
	{
		return EulerAngles(-_Roll, -_Yaw, -_Pitch);
	}

	/*
	*	Returns the data, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD float32 *const RESTRICT Data() NOEXCEPT
	{
		return &_Roll;
	}

	/*
	*	Returns the data, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const float32 *const RESTRICT Data() const NOEXCEPT
	{
		return &_Roll;
	}

};