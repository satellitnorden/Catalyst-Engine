#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class AxisAlignedBoundingBox final
{

public:

	//The minimum corner.
	Vector3<float> _Minimum;

	//The maximum corner.
	Vector3<float> _Maximum;

	/*
	*	Calcules the center of an axis-aligned bounding box.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> CalculateCenter(const AxisAlignedBoundingBox &box) NOEXCEPT
	{
		return box._Minimum + ((box._Maximum - box._Minimum) * 0.5f);
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
	FORCE_INLINE constexpr AxisAlignedBoundingBox(const Vector3<float> &initialMinimum, const Vector3<float> &initialMaximum) NOEXCEPT
		:
		_Minimum(initialMinimum),
		_Maximum(initialMaximum)
	{

	}

};