#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Rendering.
#include <Rendering/Native/Frustum.h>

namespace Culling
{

	/*
	*	Returns if the given box is within the given frustum.
	*/
	FORCE_INLINE NO_DISCARD bool IsWithinFrustum(const AxisAlignedBoundingBox3D &box, const Frustum &frustum) NOEXCEPT
	{
		for (uint8 i = 0; i < 6; ++i)
		{
			Vector3<float32> axis;

			if (frustum._Planes[i]._X < 0.0f)
			{
				axis._X = box._Minimum._X;
			}

			else
			{
				axis._X = box._Maximum._X;
			}

			if (frustum._Planes[i]._Y < 0.0f)
			{
				axis._Y = box._Minimum._Y;
			}

			else
			{
				axis._Y = box._Maximum._Y;
			}

			if (frustum._Planes[i]._Z < 0.0f)
			{
				axis._Z = box._Minimum._Z;
			}

			else
			{
				axis._Z = box._Maximum._Z;
			}

			const Vector3<float32> plane_normal{ Vector3<float32>(frustum._Planes[i]._X, frustum._Planes[i]._Y, frustum._Planes[i]._Z) };

			if (Vector3<float32>::DotProduct(plane_normal, axis) + frustum._Planes[i]._W < 0.0f)
			{
				return false;
			}
		}

		return true;
	}

}