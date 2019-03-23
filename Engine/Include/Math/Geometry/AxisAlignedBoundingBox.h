#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector3.h>

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
	static Vector3<float> CalculateCenter(const AxisAlignedBoundingBox &box) NOEXCEPT
	{
		return box._Minimum + ((box._Maximum - box._Minimum) * 0.5f);
	}

	/*
	*	Default constructor.
	*/
	AxisAlignedBoundingBox() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the minimum and maximum values.
	*/
	AxisAlignedBoundingBox(const Vector3<float> &initialMinimum, const Vector3<float> &initialMaximum) NOEXCEPT
		:
		_Minimum(initialMinimum),
		_Maximum(initialMaximum)
	{

	}

};