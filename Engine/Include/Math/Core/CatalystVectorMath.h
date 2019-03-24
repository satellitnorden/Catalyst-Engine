#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector2.h>
#include <Math/General/Vector3.h>
#include <Math/General/Vector4.h>
#include <Math/Geometry/Plane.h>
#include <Math/Geometry/Ray.h>
#include <Math/Geometry/Sphere.h>

class CatalystVectorMath final
{

public:

	/*
	*	Performs a box-box intersection and return whether or not there was an intersection.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool BoxIntersection(const AxisAlignedBoundingBox &box1, const AxisAlignedBoundingBox &box2) NOEXCEPT
	{
		return	box1._Minimum._X <= box2._Maximum._X
				&& box1._Maximum._X >= box2._Minimum._X
				&& box1._Minimum._Y <= box2._Maximum._Y
				&& box1._Maximum._Y >= box2._Minimum._Y
				&& box1._Minimum._Z <= box2._Maximum._Z
				&& box1._Maximum._Z >= box2._Minimum._Z;
	}

	/*
	*	Performs a ray-box intersection and returns whether or not there was an intersection.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayBoxIntersection(const Ray &ray, const AxisAlignedBoundingBox &box) NOEXCEPT
	{
		//Pre-calculate the inverse direction of the ray to avoid costly divisions.
		const Vector3<float> inverseDirection{ Vector3<float>(1.0f) / ray._Direction };

		float minimum{ 0.0f };
		float maximum{ 0.0f };

		const float minimumX{ (box._Minimum._X - ray._Origin._X) * inverseDirection._X };
		const float maximumX{ (box._Maximum._X - ray._Origin._X) * inverseDirection._X };

		minimum = CatalystBaseMath::Minimum<float>(minimumX, maximumX);
		maximum = CatalystBaseMath::Maximum<float>(minimumX, maximumX);

		const float minimumY{ (box._Minimum._Y - ray._Origin._Y) * inverseDirection._Y };
		const float maximumY{ (box._Maximum._Y - ray._Origin._Y) * inverseDirection._Y };

		minimum = CatalystBaseMath::Maximum<float>(minimum, CatalystBaseMath::Minimum<float>(minimumY, maximumY));
		maximum = CatalystBaseMath::Minimum<float>(maximum, CatalystBaseMath::Maximum<float>(minimumY, maximumY));

		const float minimumZ{ (box._Minimum._Z - ray._Origin._Z) * inverseDirection._Z };
		const float maximumZ{ (box._Maximum._Z - ray._Origin._Z) * inverseDirection._Z };

		minimum = CatalystBaseMath::Maximum<float>(minimum, CatalystBaseMath::Minimum<float>(minimumZ, maximumZ));
		maximum = CatalystBaseMath::Minimum<float>(maximum, CatalystBaseMath::Maximum<float>(minimumZ, maximumZ));

		return maximum >= minimum;
	}

	/*
	*	Performs a ray-plane intersection. Returns if there was an intersection and will, in the event of an intersection, return the intersection point as well.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayPlaneIntersection(	const Ray &ray,
																		const Plane &plane,
																		Vector3<float> *const RESTRICT intersectionPoint) NOEXCEPT
	{
		if (Vector3<float>::DotProduct(ray._Direction, plane._Normal) >= 0.0f)
		{
			return false;
		}

		else
		{
			*intersectionPoint = Vector3<float>::DotProduct(plane._Position - ray._Origin, plane._Normal) / Vector3<float>::DotProduct(ray._Direction, plane._Normal) * ray._Direction + ray._Origin;

			return true;
		}
	}

	/*
	*   Performs a ray-sphere intersection. Returns if there was an intersection and will, in the event of an intersection, return the intersection point as well.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RaySphereIntersection(	const Ray &ray,
																			const Sphere &sphere,
																			Vector3<float> *const RESTRICT intersectionPoint) NOEXCEPT
	{
		const Vector3<float> L{ ray._Origin - sphere._Position };
		float B = Vector3<float>::DotProduct(ray._Direction, L) * 2.0f;
		float C = Vector3<float>::DotProduct(L, L) - sphere._Radius * sphere._Radius;

		float discriminant = B * B - 4.0f * C;

		if (discriminant <= 0.0f)
		{
			return false;
		}

		else
		{
			discriminant = CatalystBaseMath::SquareRoot(discriminant);
		}

		float T0 = -B + discriminant;
		float T1 = -B - discriminant;

		if (T0 <= 0.0f && T1 <= 0.0f)
		{
			return false;
		}

		float T = T0 < T1 ? T0 * 0.5f : T1 * 0.5f;

		*intersectionPoint = ray._Origin + T * ray._Direction;

		return true;
	}

};