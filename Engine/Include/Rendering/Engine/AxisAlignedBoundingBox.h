#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Vector3.h>

class AxisAlignedBoundingBox final
{

public:

	//The minimum corner.
	Vector3 _Minimum;

	//The maximum corner.
	Vector3 _Maximum;

	/*
	*	Default constructor.
	*/
	AxisAlignedBoundingBox() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the minimum and maximum values.
	*/
	AxisAlignedBoundingBox(const Vector3 &initialMinimum, const Vector3 &initialMaximum) NOEXCEPT
		:
		_Minimum(initialMinimum),
		_Maximum(initialMaximum)
	{

	}

	/*
	*	Calcules the center of an axis-aligned bounding box.
	*/
	static Vector3 CalculateCenter(const AxisAlignedBoundingBox &box) NOEXCEPT
	{
		return box._Minimum + ((box._Maximum - box._Minimum) * 0.5f);
	}

};