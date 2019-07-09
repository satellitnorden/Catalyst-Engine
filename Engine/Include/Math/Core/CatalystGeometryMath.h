#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Geometry/Plane.h>
#include <Math/Geometry/Ray.h>
#include <Math/Geometry/Sphere.h>
#include <Math/Geometry/Triangle.h>

class CatalystGeometryMath final
{

public:

	/*
	*	Calculates the barycentric coordinates for a point in a triangle.
	*/
	FORCE_INLINE constexpr static void CalculateBarycentricCoordinates(	const Triangle &triangle,
																		const Vector3<float> &point,
																		Vector3<float> *const RESTRICT barycentricCoordinates) NOEXCEPT
	{
		const Vector3<float> v0{ triangle._Vertex2 - triangle._Vertex1 };
		const Vector3<float> v1{ triangle._Vertex3 - triangle._Vertex1 };
		const Vector3<float> v2{ point - triangle._Vertex1 };

		const float d00{ Vector3<float>::DotProduct(v0, v0) };
		const float d01{ Vector3<float>::DotProduct(v0, v1) };
		const float d11{ Vector3<float>::DotProduct(v1, v1) };
		const float d20{ Vector3<float>::DotProduct(v2, v0) };
		const float d21{ Vector3<float>::DotProduct(v2, v1) };

		const float denom{ 1.0f / (d00 * d11 - d01 * d01) };

		barycentricCoordinates->_Y = (d11 * d20 - d01 * d21) * denom;
		barycentricCoordinates->_Z = (d00 * d21 - d01 * d20) * denom;
		barycentricCoordinates->_X = 1.0f - barycentricCoordinates->_Y - barycentricCoordinates->_Z;
	}

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
	FORCE_INLINE constexpr static NO_DISCARD bool RayBoxIntersection(	const Ray &ray,
																		const AxisAlignedBoundingBox &box,
																		Vector3<float> *const RESTRICT intersectionPoint) NOEXCEPT
	{
		//Pre-calculate the reciprocal of the direction of the ray to avoid costly divisions later on.
		const Vector3<float> directionReciprocal{ Vector3<float>(1.0f) / ray._Direction };

		//Find the minimum/maximum.
		float minimum{ 0.0f };
		float maximum{ FLOAT_MAXIMUM };

		//Test the X-axis slab.
		float minimumX{ (box._Minimum._X - ray._Origin._X) * directionReciprocal._X };
		float maximumX{ (box._Maximum._X - ray._Origin._X) * directionReciprocal._X };

		if (minimumX > maximumX)
		{
			Swap(&minimumX, &maximumX);
		}

		minimum = CatalystBaseMath::Maximum<float>(minimum, minimumX);
		maximum = CatalystBaseMath::Minimum<float>(maximum, maximumX);

		if (minimum > maximum)
		{
			return false;
		}

		//Test the Y-axis slab.
		float minimumY{ (box._Minimum._Y - ray._Origin._Y) * directionReciprocal._Y };
		float maximumY{ (box._Maximum._Y - ray._Origin._Y) * directionReciprocal._Y };

		if (minimumY > maximumY)
		{
			Swap(&minimumY, &maximumY);
		}

		minimum = CatalystBaseMath::Maximum<float>(minimum, minimumY);
		maximum = CatalystBaseMath::Minimum<float>(maximum, maximumY);

		if (minimum > maximum)
		{
			return false;
		}

		//Test the Z-axis slab.
		float minimumZ{ (box._Minimum._Z - ray._Origin._Z) * directionReciprocal._Z };
		float maximumZ{ (box._Maximum._Z - ray._Origin._Z) * directionReciprocal._Z };

		if (minimumZ > maximumZ)
		{
			Swap(&minimumZ, &maximumZ);
		}

		minimum = CatalystBaseMath::Maximum<float>(minimum, minimumZ);
		maximum = CatalystBaseMath::Minimum<float>(maximum, maximumZ);

		if (minimum > maximum)
		{
			return false;
		}

		//All slabs succeeded!
		if (intersectionPoint)
		{
			*intersectionPoint = ray._Origin + ray._Direction * minimum;
		}

		return true;
	}

	/*
	*	Performs a ray-plane intersection. Returns if there was an intersection and will, in the event of an intersection, return the intersection point as well.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayPlaneIntersection(	const Ray &ray,
																		const Plane &plane,
																		float *const RESTRICT intersectionDistance) NOEXCEPT
	{
		if (Vector3<float>::DotProduct(ray._Direction, plane._Normal) >= 0.0f)
		{
			return false;
		}

		else
		{
			if (intersectionDistance)
			{
				*intersectionDistance = Vector3<float>::DotProduct(plane._Position - ray._Origin, plane._Normal) / Vector3<float>::DotProduct(ray._Direction, plane._Normal);
			}

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

		if (intersectionPoint)
		{
			*intersectionPoint = ray._Origin + T * ray._Direction;
		}

		return true;
	}

	/*
	*	Performs a ray-triangle intersection and return whether or not there was an intersection.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayTriangleIntersection(	const Ray &ray,
																			const Triangle &triangle,
																			Vector3<float> *const RESTRICT intersectionPoint) NOEXCEPT
	{
		constexpr float EPSILON{ 0.0000001f };

		Vector3<float> edge1, edge2, h, s, q;
		float a{ 0.0f }, f{ 0.0f }, u{ 0.0f }, v{ 0.0f };

		edge1 = triangle._Vertex2 - triangle._Vertex1;
		edge2 = triangle._Vertex3 - triangle._Vertex1;

		h = Vector3<float>::CrossProduct(ray._Direction, edge2);
		a = Vector3<float>::DotProduct(edge1, h);

		if (a > -EPSILON && a < EPSILON)
		{
			return false;
		}

		f = 1.0f / a;
		s = ray._Origin - triangle._Vertex1;

		u = f * Vector3<float>::DotProduct(s, h);

		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		q = Vector3<float>::CrossProduct(s, edge1);
		v = f * Vector3<float>::DotProduct(ray._Direction, q);

		if (v < 0.0 || u + v > 1.0)
		{
			return false;
		}
			
		float t = f * Vector3<float>::DotProduct(edge2, q);

		if (t > EPSILON)
		{
			if (intersectionPoint)
			{
				*intersectionPoint = ray._Origin + ray._Direction * t;
			}

			return true;
		}

		else
		{
			return false;
		}
	}

};