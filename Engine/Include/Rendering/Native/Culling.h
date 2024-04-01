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
	*	Calculates a distance based culling score, taking into account how large the object is.
	*	Takes in an axis aligned bounding box, a position and some multipliers for the dimensions.
	*	The larger the multipliers, the higher the score will be.
	*	Negative scores indicate that the object should be culled.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculateDistanceBasedCullingScore
	(
		const AxisAlignedBoundingBox3D &box,
		const Vector3<float32> &dimension_multipliers,
		const float32 distance
	) NOEXCEPT
	{
		//Cache the dimensions.
		const Vector3<float32> dimensions{ box.Dimensions() };

		//Calculate the dimensions score.
		const float32 dimensions_score
		{
			dimensions._X * dimension_multipliers._X
			+ dimensions._Y * dimension_multipliers._Y
			+ dimensions._Z * dimension_multipliers._Z
		};

		//Calculate the score. Normalize it so that it falls in the [-INFINITY, 1.0f] range.
		const float32 score{ (dimensions_score - distance) / dimensions_score };

		//Return the score!
		return score;
	}

	/*
	*	Calculates the screen coverage of the given axis aligned bounding box.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculateScreenCoverage(const AxisAlignedBoundingBox3D &box, const Matrix4x4 &clip_matrix) NOEXCEPT
	{
		//Calculate the minimum/maximum screen coordinates from the corners of the relative axis aligned bounding box.
		Vector2<float32> minimum_screen_coordinate{ FLOAT32_MAXIMUM, FLOAT32_MAXIMUM };
		Vector2<float32> maximum_screen_coordinate{ -FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM };

		const StaticArray<Vector3<float32>, 8> corners
		{
			Vector3<float32>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z),
			Vector3<float32>(box._Minimum._X, box._Minimum._Y, box._Maximum._Z),
			Vector3<float32>(box._Minimum._X, box._Maximum._Y, box._Minimum._Z),
			Vector3<float32>(box._Minimum._X, box._Maximum._Y, box._Maximum._Z),

			Vector3<float32>(box._Maximum._X, box._Minimum._Y, box._Minimum._Z),
			Vector3<float32>(box._Maximum._X, box._Minimum._Y, box._Maximum._Z),
			Vector3<float32>(box._Maximum._X, box._Maximum._Y, box._Minimum._Z),
			Vector3<float32>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z)
		};

		for (uint8 corner_index{ 0 }; corner_index < 8; ++corner_index)
		{
			Vector4<float32> screen_space_position{ clip_matrix * Vector4<float32>(corners[corner_index], 1.0f) };
			const float32 screen_space_position_reciprocal{ 1.0f / screen_space_position._W };

			Vector2<float32> screen_coordinate{ screen_space_position._X * screen_space_position_reciprocal, screen_space_position._Y * screen_space_position_reciprocal };

			screen_coordinate._X = screen_coordinate._X * 0.5f + 0.5f;
			screen_coordinate._Y = screen_coordinate._Y * 0.5f + 0.5f;

			minimum_screen_coordinate = Vector2<float32>::Minimum(minimum_screen_coordinate, screen_coordinate);
			maximum_screen_coordinate = Vector2<float32>::Maximum(maximum_screen_coordinate, screen_coordinate);
		}

		//Calculate the screen coverage.
		return CatalystBaseMath::Minimum<float32>((maximum_screen_coordinate._X - minimum_screen_coordinate._X) * (maximum_screen_coordinate._Y - minimum_screen_coordinate._Y), 1.0f);
	}

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