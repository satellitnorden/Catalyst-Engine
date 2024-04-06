#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Capsule.h>
#include <Math/Geometry/Circle.h>
#include <Math/Geometry/LineSegment2D.h>
#include <Math/Geometry/Plane.h>
#include <Math/Geometry/Ray.h>
#include <Math/Geometry/Sphere.h>
#include <Math/Geometry/Triangle.h>

class CatalystGeometryMath final
{

public:

	/*
	*	Performs a box-box intersection and return whether or not there was an intersection.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool BoxBoxIntersection(const AxisAlignedBoundingBox3D& box1, const AxisAlignedBoundingBox3D& box2) NOEXCEPT
	{
		return	box1._Minimum._X <= box2._Maximum._X
			&& box1._Maximum._X >= box2._Minimum._X
			&& box1._Minimum._Y <= box2._Maximum._Y
			&& box1._Maximum._Y >= box2._Minimum._Y
			&& box1._Minimum._Z <= box2._Maximum._Z
			&& box1._Maximum._Z >= box2._Minimum._Z;
	}

	/*
	*	Calculates the barycentric coordinates for a point in a triangle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float32> CalculateBarycentricCoordinates(const Triangle &triangle, const Vector3<float> &point) NOEXCEPT
	{
		const Vector3<float> v0{ triangle._Vertices[1] - triangle._Vertices[0] };
		const Vector3<float> v1{ triangle._Vertices[2] - triangle._Vertices[0] };
		const Vector3<float> v2{ point - triangle._Vertices[0] };

		const float d00{ Vector3<float>::DotProduct(v0, v0) };
		const float d01{ Vector3<float>::DotProduct(v0, v1) };
		const float d11{ Vector3<float>::DotProduct(v1, v1) };
		const float d20{ Vector3<float>::DotProduct(v2, v0) };
		const float d21{ Vector3<float>::DotProduct(v2, v1) };

		const float denom{ 1.0f / (d00 * d11 - d01 * d01) };

		Vector3<float32> output;

		output._Y = (d11 * d20 - d01 * d21) * denom;
		output._Z = (d00 * d21 - d01 * d20) * denom;
		output._X = 1.0f - output._Y - output._Z;

		return output;
	}

	/*
	*	Calculates the cosinus hemisphere vector given a coordinate.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> CalculateHemisphereCosinus(const Vector2<float32> &coordinate) NOEXCEPT
	{
		//Map the 2D coordinate onto the hemisphere.
		const float32 phi{ coordinate._Y * 2.0f * BaseMathConstants::PI };
		const float32 cos_theta{ BaseMath::SquareRoot(1.0f - coordinate._X) };
		const float32 sin_theta{ BaseMath::SquareRoot(1.0f - cos_theta * cos_theta) };

		//Return the cosinus hemisphere coordinate.
		return Vector3<float32>(BaseMath::Cosine(phi) * sin_theta, BaseMath::Sine(phi) * sin_theta, cos_theta);
	}

	/*
	*	Calculates the uniform hemisphere vector given a coordinate.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> CalculateHemisphereUniform(const Vector2<float32> &coordinate) NOEXCEPT
	{
		//Map the 2D coordinate onto the hemisphere.
		const float32 phi{ coordinate._Y * 2.0f * BaseMathConstants::PI };
		const float32 cos_theta{ 1.0f - coordinate._X };
		const float32 sin_theta{ BaseMath::SquareRoot(1.0f - cos_theta * cos_theta) };

		//Return the cosinus hemisphere coordinate.
		return Vector3<float32>(BaseMath::Cosine(phi) * sin_theta, BaseMath::Sine(phi) * sin_theta, cos_theta);
	}

	/*
	*	Performs a circle-circle overlap. Returns if there was an overlap.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool CircleCircleOverlap(const Circle &A, const Circle &B) NOEXCEPT
	{
		const float32 distance_squared{ Vector2<float32>::LengthSquared(A._Position - B._Position) };
		const float32 radii{ A._Radius + B._Radius };
		const float32 radii_squared{ radii * radii };
		return distance_squared <= radii_squared;
	}

	/*
	*	Returns the closest point on a 2D line segment to the given point.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector2<float32> ClosestPointOnLineSegment(const LineSegment2D &line_segment, const Vector2<float32> point) NOEXCEPT
	{
		const Vector2<float32> end_to_start{ line_segment._End - line_segment._Start };
		const Vector2<float32> point_to_start{ point - line_segment._Start };

		const float32 end_to_start_length{ Vector2<float32>::LengthSquared(end_to_start) };
		const float32 end_to_start_length_reciprocal{ 1.0f / end_to_start_length };

		const float32 projection{ Vector2<float32>::DotProduct(point_to_start, end_to_start) * end_to_start_length_reciprocal };

		return BaseMath::LinearlyInterpolate(line_segment._Start, line_segment._End, BaseMath::Clamp<float32>(projection, 0.0f, 1.0f));
	}

	/*
	*	Performs a ray-box intersection and returns whether or not there was an intersection.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayBoxIntersection(	const Ray &ray,
																		const AxisAlignedBoundingBox3D &box,
																		float *const RESTRICT intersection_distance) NOEXCEPT
	{
		//Find the minimum/maximum.
		float32 minimum{ -FLOAT32_MAXIMUM };
		float32 maximum{ FLOAT32_MAXIMUM };

		//Test the X-axis slab.
		minimum = BaseMath::Maximum<float32>(minimum, (box[1 - ray._Signs[0]]._X - ray._Origin._X) * ray._Reciprocals._X);
		maximum = BaseMath::Minimum<float32>(maximum, (box[ray._Signs[0]]._X - ray._Origin._X) * ray._Reciprocals._X);

		if (minimum > maximum)
		{
			return false;
		}

		//Test the Y-axis slab.
		minimum = BaseMath::Maximum<float32>(minimum, (box[1 - ray._Signs[1]]._Y - ray._Origin._Y) * ray._Reciprocals._Y);
		maximum = BaseMath::Minimum<float32>(maximum, (box[ray._Signs[1]]._Y - ray._Origin._Y) * ray._Reciprocals._Y);

		if (minimum > maximum)
		{
			return false;
		}

		//Test the Z-axis slab.
		minimum = BaseMath::Maximum<float32>(minimum, (box[1 - ray._Signs[2]]._Z - ray._Origin._Z) * ray._Reciprocals._Z);
		maximum = BaseMath::Minimum<float32>(maximum, (box[ray._Signs[2]]._Z - ray._Origin._Z) * ray._Reciprocals._Z);

		if (minimum > maximum)
		{
			return false;
		}

		//Maximum needs to be positive.
		if (maximum <= 0.0f)
		{
			return false;
		}

		//All slabs succeeded!
		if (intersection_distance)
		{
			*intersection_distance = minimum;
		}

		return true;
	}

	/*
	*	Performs a ray-plane intersection. Returns if there was an intersection and will, in the event of an intersection, return the intersection point as well.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayPlaneIntersection(	const Ray &ray,
																		const Plane &plane,
																		float *const RESTRICT intersection_distance) NOEXCEPT
	{
		if (Vector3<float>::DotProduct(ray._Direction, plane._Normal) >= 0.0f)
		{
			return false;
		}

		else
		{
			const float distance{ Vector3<float>::DotProduct(plane._Position - ray._Origin, plane._Normal) / Vector3<float>::DotProduct(ray._Direction, plane._Normal) };

			if (intersection_distance)
			{
				*intersection_distance = distance;
			}

			return true;
		}
	}

	/*
	*   Performs a ray-sphere intersection. Returns if there was an intersection and will, in the event of an intersection, return the intersection point as well.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RaySphereIntersection(	const Ray &ray,
																			const Sphere &sphere,
																			float *const RESTRICT intersection_distance) NOEXCEPT
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
			discriminant = BaseMath::SquareRoot(discriminant);
		}

		float T0 = -B + discriminant;
		float T1 = -B - discriminant;

		if (T0 <= 0.0f && T1 <= 0.0f)
		{
			return false;
		}

		float T = T0 < T1 ? T0 * 0.5f : T1 * 0.5f;

		if (intersection_distance)
		{
			*intersection_distance = T;
		}

		return true;
	}

	/*
	*	Performs a ray-triangle intersection and return whether or not there was an intersection.
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool RayTriangleIntersection(	const Ray &ray,
																			const Triangle &triangle,
																			float *const RESTRICT intersection_distance) NOEXCEPT
	{
		constexpr float EPSILON{ 0.0000001f };

		Vector3<float> edge1, edge2, h, s, q;
		float a{ 0.0f }, f{ 0.0f }, u{ 0.0f }, v{ 0.0f };

		edge1 = triangle._Vertices[1] - triangle._Vertices[0];
		edge2 = triangle._Vertices[2] - triangle._Vertices[0];

		h = Vector3<float>::CrossProduct(ray._Direction, edge2);
		a = Vector3<float>::DotProduct(edge1, h);

		if (a > -EPSILON && a < EPSILON)
		{
			return false;
		}

		f = 1.0f / a;
		s = ray._Origin - triangle._Vertices[0];

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
			*intersection_distance = t;

			return true;
		}

		else
		{
			return false;
		}
	}

};