#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class AxisAlignedBoundingBox2D final
{

public:

	union
	{
		struct
		{
			//The minimum corner.
			Vector2<float32> _Minimum;

			//The maximum corner.
			Vector2<float32> _Maximum;
		};

		//The corners.
		StaticArray<Vector2<float32>, 2> _Corners;
	};

	/*
	*	Calcules the center of an axis-aligned bounding box.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector2<float32> CalculateCenter(const AxisAlignedBoundingBox2D &box) NOEXCEPT
	{
		return box._Minimum + ((box._Maximum - box._Minimum) * 0.5f);
	}

	/*
	*	Calcules the closest point inside an axis-aligned bounding box relative to the specified position.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector2<float32> GetClosestPointInside(const AxisAlignedBoundingBox2D &box, const Vector2<float32> &position) NOEXCEPT
	{
		return Vector2<float32>(CatalystBaseMath::Clamp<float32>(position._X, box._Minimum._X, box._Maximum._X),
								CatalystBaseMath::Clamp<float32>(position._Y, box._Minimum._Y, box._Maximum._Y));
	}

	/*
	*	Returns the union of two axis aligned bounding boxes.
	*/
	FORCE_INLINE constexpr static NO_DISCARD AxisAlignedBoundingBox2D Union(const AxisAlignedBoundingBox2D &first, const AxisAlignedBoundingBox2D &second) NOEXCEPT
	{
		return AxisAlignedBoundingBox2D(Vector2<float32>::Minimum(first._Minimum, second._Minimum),
										Vector2<float32>::Maximum(first._Maximum, second._Maximum));
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr AxisAlignedBoundingBox2D() NOEXCEPT
		:
		_Minimum(FLOAT32_MAXIMUM, FLOAT32_MAXIMUM),
		_Maximum(-FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM)
	{

	}

	/*
	*	Constructor taking the minimum and maximum values.
	*/
	FORCE_INLINE constexpr AxisAlignedBoundingBox2D(const Vector2<float32> &initial_minimum, const Vector2<float32> & initial_maximum) NOEXCEPT
		:
		_Minimum(initial_minimum),
		_Maximum(initial_maximum)
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE constexpr AxisAlignedBoundingBox2D(const AxisAlignedBoundingBox2D &other) NOEXCEPT
		:
		_Minimum(other._Minimum),
		_Maximum(other._Maximum)
	{
		
	}

	/*
	*	Addition by Vector2<float> operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD AxisAlignedBoundingBox2D operator+(const Vector2<float32> &vector) const NOEXCEPT
	{
		return AxisAlignedBoundingBox2D(_Minimum + vector, _Maximum + vector);
	}

	/*
	*	Assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const AxisAlignedBoundingBox2D &other) NOEXCEPT
	{
		_Minimum = other._Minimum;
		_Maximum = other._Maximum;
	}

	/*
	*	Equality operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool operator==(const AxisAlignedBoundingBox2D &other) NOEXCEPT
	{
		return	_Minimum == other._Minimum
				&& _Maximum == other._Maximum;
	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE constexpr NO_DISCARD const Vector2<float32> &operator[](const uint64 index) const NOEXCEPT
	{
		return _Corners[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE constexpr  Vector2<float32> &operator[](const uint64 index) NOEXCEPT
	{
		return _Corners[index];
	}

	/*
	*	Returns the dimensions.
	*/
	FORCE_INLINE NO_DISCARD Vector2<float32> Dimensions() const NOEXCEPT
	{
		return _Maximum - _Minimum;
	}

	/*
	*	Expands this axis aligned bounding box with the position.
	*/
	FORCE_INLINE constexpr void Expand(const Vector2<float32> &position) NOEXCEPT
	{
		_Minimum = Vector2<float32>::Minimum(_Minimum, position);
		_Maximum = Vector2<float32>::Maximum(_Maximum, position);
	}

	/*
*	Expands this axis aligned bounding box with the box.
*/
	FORCE_INLINE constexpr void Expand(const AxisAlignedBoundingBox2D& box) NOEXCEPT
	{
		Expand(box._Minimum);
		Expand(box._Maximum);
	}

	/*
	*	Returns whether or not a position is inside this axis aligned bounding box.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsInside(const Vector2<float32> &position) const NOEXCEPT
	{
		return	position._X >= _Minimum._X && position._X < _Maximum._X
				&& position._Y >= _Minimum._Y && position._Y < _Maximum._Y;
	}

	/*
	*	Invalides this axis aligned bounding box.
	*/
	FORCE_INLINE constexpr void Invalidate() NOEXCEPT
	{
		_Minimum = Vector2<float32>(FLOAT32_MAXIMUM, FLOAT32_MAXIMUM);
		_Maximum = Vector2<float32>(-FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM);
	}

	/*
	*	Returns if this axis aligned bounding box is valid.
	*/
	FORCE_INLINE constexpr NO_DISCARD bool IsValid() const NOEXCEPT
	{
		return	_Minimum._X <= _Maximum._X
				&& _Minimum._Y <= _Maximum._Y;
	}

};