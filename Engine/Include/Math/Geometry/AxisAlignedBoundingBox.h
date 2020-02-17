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
	*	Addition by Vector3<float> operator overload.
	*/
	FORCE_INLINE NO_DISCARD AxisAlignedBoundingBox operator+(const Vector3<float> &vector) const NOEXCEPT
	{
		return AxisAlignedBoundingBox(_Minimum + vector, _Maximum + vector);
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
*	Expands this axis aligned bounding box with the box.
*/
	FORCE_INLINE constexpr void Expand(const AxisAlignedBoundingBox& box) NOEXCEPT
	{
		Expand(box._Minimum);
		Expand(box._Maximum);
	}

	/*
	*	Returns whether or not a position is inside this axis aligned bounding box.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsInside(const Vector3<float> &position) const NOEXCEPT
	{
		return	position._X >= _Minimum._X && position._X < _Maximum._X
				&& position._Y >= _Minimum._Y && position._Y < _Maximum._Y
				&& position._Z >= _Minimum._Z && position._Z < _Maximum._Z;
	}

	/*
	*	Returns whether or not a position is inside this axis aligned bounding box on the X and Y axis.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsInsideXY(const Vector3<float> &position) const NOEXCEPT
	{
		return	position._X >= _Minimum._X && position._X < _Maximum._X
				&& position._Y >= _Minimum._Y && position._Y < _Maximum._Y;
	}

	/*
	*	Returns whether or not a position is inside this axis aligned bounding boxon the X and Z axis.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsInsideXZ(const Vector3<float> &position) const NOEXCEPT
	{
		return	position._X >= _Minimum._X && position._X < _Maximum._X
				&& position._Z >= _Minimum._Z && position._Z < _Maximum._Z;
	}

	/*
	*	Returns whether or not a position is inside this axis aligned bounding box on the Y and Z axis.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsInsideYZ(const Vector3<float> &position) const NOEXCEPT
	{
		return	position._Y >= _Minimum._Y && position._Y < _Maximum._Y
				&& position._Z >= _Minimum._Z && position._Z < _Maximum._Z;
	}

	/*
	*	Invalides this axis aligned bounding box.
	*/
	FORCE_INLINE constexpr void Invalidate() NOEXCEPT
	{
		_Minimum = Vector3<float>(FLOAT_MAXIMUM, FLOAT_MAXIMUM, FLOAT_MAXIMUM);
		_Maximum = Vector3<float>(-FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM);
	}

	/*
	*	Returns if this axis aligned bounding box is valid.
	*/
	FORCE_INLINE NO_DISCARD bool IsValid() const NOEXCEPT
	{
		return	_Minimum._X <= _Maximum._X
				&& _Minimum._Y <= _Maximum._Y
				&& _Minimum._Z <= _Maximum._Z;
	}
};