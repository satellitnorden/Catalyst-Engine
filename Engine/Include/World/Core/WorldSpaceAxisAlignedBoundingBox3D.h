#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//World.
#include <World/Core/WorldPosition.h>

class WorldSpaceAxisAlignedBoundingBox3D final
{

public:

	//The minimum.
	WorldPosition _Minimum;

	//The maximum.
	WorldPosition _Maximum;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE explicit WorldSpaceAxisAlignedBoundingBox3D() NOEXCEPT
	{

	}

	/*
	*	Construct taking the minimum/maximum values.
	*/
	FORCE_INLINE explicit WorldSpaceAxisAlignedBoundingBox3D(const WorldPosition &initial_minimum, const WorldPosition &initial_maximum) NOEXCEPT
		:
		_Minimum(initial_minimum),
		_Maximum(initial_maximum)
	{

	}

	/*
	*	Returns the local axis aligned bounding box.
	*/
	FORCE_INLINE NO_DISCARD AxisAlignedBoundingBox3D GetLocalAxisAlignedBoundingBox() const NOEXCEPT
	{
		return AxisAlignedBoundingBox3D(_Minimum.GetLocalPosition(), _Maximum.GetLocalPosition());
	}

	/*
	*	Returns the absolute axis aligned bounding box.
	*/
	FORCE_INLINE NO_DISCARD AxisAlignedBoundingBox3D GetAbsoluteAxisAlignedBoundingBox() const NOEXCEPT
	{
		return AxisAlignedBoundingBox3D(_Minimum.GetAbsolutePosition(), _Maximum.GetAbsolutePosition());
	}

	/*
	*	Returns the relative axis aligned bounding box.
	*/
	FORCE_INLINE NO_DISCARD AxisAlignedBoundingBox3D GetRelativeAxisAlignedBoundingBox(const Vector3<int32>& cell) const NOEXCEPT
	{
		return AxisAlignedBoundingBox3D(_Minimum.GetRelativePosition(cell), _Maximum.GetRelativePosition(cell));
	}

};