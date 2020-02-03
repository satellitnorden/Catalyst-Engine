#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class AxisAlignedBoundingBox final
{

public:

	//The minimum corner.
	Vector3<float> _Minimum{ FLOAT_MAXIMUM, FLOAT_MAXIMUM, FLOAT_MAXIMUM };

	//The maximum corner.
	Vector3<float> _Maximum{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

	/*
	*	Calcules the center of an axis-aligned bounding box.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> CalculateCenter(const AxisAlignedBoundingBox &box) NOEXCEPT
	{
		return box._Minimum + ((box._Maximum - box._Minimum) * 0.5f);
	}

	/*
	*	Calcules the closest point on an axis-aligned bounding box relative to the specified position.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> GetClosestPoint(const AxisAlignedBoundingBox &box, const Vector3<float> &position) NOEXCEPT
	{
		return Vector3<float>(	CatalystBaseMath::Clamp<float>(position._X, box._Minimum._X, box._Maximum._X),
								CatalystBaseMath::Clamp<float>(position._Y, box._Minimum._Y, box._Maximum._Y),
								CatalystBaseMath::Clamp<float>(position._Z, box._Minimum._Z, box._Maximum._Z));
	}

	/*
	*	Returns the union of two axis aligned bounding boxes.
	*/
	FORCE_INLINE constexpr static NO_DISCARD AxisAlignedBoundingBox Union(const AxisAlignedBoundingBox &first, const AxisAlignedBoundingBox &second) NOEXCEPT
	{
		return AxisAlignedBoundingBox(	Vector3<float>::Minimum(first._Minimum, second._Minimum),
										Vector3<float>::Maximum(first._Maximum, second._Maximum));
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr AxisAlignedBoundingBox() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the minimum and maximum values.
	*/
	FORCE_INLINE constexpr AxisAlignedBoundingBox(const Vector3<float> &initial_minimum, const Vector3<float> & initial_maximum) NOEXCEPT
		:
		_Minimum(initial_minimum),
		_Maximum(initial_maximum)
	{

	}

	/*
	*	Expands this axis aligned bounding box with the position.
	*/
	FORCE_INLINE constexpr void Expand(const Vector3<float> &position) NOEXCEPT
	{
		_Minimum = Vector3<float>::Minimum(_Minimum, position);
		_Maximum = Vector3<float>::Maximum(_Maximum, position);
	}

	/*
	*	Returns whether or not a position is inside this axis aligned bounding box.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsInside(const Vector3<float> &position) NOEXCEPT
	{
		return	position._X >= _Minimum._X && position._X < _Maximum._X
				&& position._Y >= _Minimum._Y && position._Y < _Maximum._Y
				&& position._Z >= _Minimum._Z && position._Z < _Maximum._Z;
	}

#if defined(CATALYST_CONFIGURATION_DEBUG)
	/*
	*	Verifies this axis-aligned bound box.
	*/
	FORCE_INLINE void Verify() const NOEXCEPT
	{
		ASSERT(_Minimum._X <= _Maximum._X, "X-axis is wrong!");
		ASSERT(_Minimum._Y <= _Maximum._Y, "Y-axis is wrong!");
		ASSERT(_Minimum._Z <= _Maximum._Z, "Z-axis is wrong!");
	}
#endif
};