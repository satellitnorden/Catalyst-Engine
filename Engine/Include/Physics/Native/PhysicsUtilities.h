#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

namespace PhysicsUtilities
{

	/*
	*	Returns whether or not a set of points with accompanying indices represents a convex model.
	*/
	FORCE_INLINE NO_DISCARD bool IsConvexSetOfPoints(const DynamicArray<Vector3<float32>> &points, const DynamicArray<uint32> &indices) NOEXCEPT
	{
        for (uint64 i{ 0 }, size{ indices.Size() }; i < size; i += 3)
        {
            Vector3<float32> A{ points[indices[i + 0]] };
            Vector3<float32> B{ points[indices[i + 1]] };
            Vector3<float32> C{ points[indices[i + 2]] };

            B -= A;
            C -= A;

            const Vector3<float32> BC_normalized{ Vector3<float32>::Normalize(Vector3<float32>::CrossProduct(B, C)) };

            const float32 check_point{ Vector3<float32>::DotProduct(Vector3<float32>(points[0]._X - A._X, points[0]._Y - A._Y, points[0]._Z - A._Z), BC_normalized) };

            for (uint64 j{ 0 }; j < points.Size(); ++j)
            {
                const float32 distance{ Vector3<float32>::DotProduct(Vector3<float32>(points[j]._X - A._X, points[j]._Y - A._Y, points[j]._Z - A._Z), BC_normalized) };

                if((CatalystBaseMath::Absolute(check_point) > 0.001f) && (CatalystBaseMath::Absolute(distance) > 0.001f) && (check_point * distance < 0.0f))
                {
                    return false;
                }
            }
        }

        return true;
	}

}