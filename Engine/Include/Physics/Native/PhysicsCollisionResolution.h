#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Capsule.h>

namespace PhysicsCollisionResolution
{
	/*
	*	Resolves a single collision. Returns if there was a collision, and if so, fills in the adjustment.
	*/
	template <typename FIRST_TYPE, typename SECOND_TYPE>
	FORCE_INLINE NO_DISCARD bool ResolveCollision(	const FIRST_TYPE &first,
													const SECOND_TYPE &second,
													Vector3<float32> *const RESTRICT position_adjustment) NOEXCEPT
	{
		return false;
	}

	/*
	*	ResolveCollision<Capsule, AxisAlignedBoundingBox3D> specialization.
	*/
	template <>
	FORCE_INLINE NO_DISCARD bool ResolveCollision<Capsule, AxisAlignedBoundingBox3D>(	const Capsule &first,
																						const AxisAlignedBoundingBox3D &second,
																						Vector3<float32> *const RESTRICT position_adjustment) NOEXCEPT
	{
		if (second.IsInside(first._Lower))
		{
			*position_adjustment = Vector3<float32>(0.0f, second._Maximum._Y - first._Lower._Y, 0.0f);

			return true;
		}

		else
		{
			return false;
		}
	}
}