#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Physics.
#include <Physics/Master/Ray.h>

class CatalystVectorMath final
{

public:

	/*
	*	Given a rotation vector, returns the forward vector.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> ForwardVector(const Vector3<float> &rotation) NOEXCEPT
	{
		return Vector3<float>::Normalize(Vector3<float>::FORWARD.Rotated(rotation));
	}

	/*
	*	Given a rotation vector, returns the right vector.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> RightVector(const Vector3<float> &rotation) NOEXCEPT
	{
		return Vector3<float>::Normalize(Vector3<float>::RIGHT.Rotated(rotation));
	}

	/*
	*	Given a rotation vector, returns the up vector.
	*/
	FORCE_INLINE constexpr static NO_DISCARD Vector3<float> UpVector(const Vector3<float> &rotation) NOEXCEPT
	{
		return Vector3<float>::Normalize(Vector3<float>::UP.Rotated(rotation));
	}

	/*
	*	Performs a box-box intersection and return whether or not there was an intersection.
	*/
	constexpr static NO_DISCARD bool BoxIntersection(const AxisAlignedBoundingBox &box1, const AxisAlignedBoundingBox &box2) NOEXCEPT
	{
		return	box1._Minimum._X <= box2._Maximum._X
				&& box1._Maximum._X >= box2._Minimum._X
				&& box1._Minimum._Y <= box2._Maximum._Y
				&& box1._Maximum._Y >= box2._Minimum._Y
				&& box1._Minimum._Z <= box2._Maximum._Z
				&& box1._Maximum._Z >= box2._Minimum._Z;
	}

	/*
	*	Performs a line-box intersection and returns whether or not there was an intersection.
	*/
	constexpr static NO_DISCARD bool LineBoxIntersection(const AxisAlignedBoundingBox &box, const Ray &ray) NOEXCEPT
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
	*	Performs a line-plane intersection and returns the intersection point.
	*/
	constexpr static NO_DISCARD Vector3<float> LinePlaneIntersection(const Vector3<float> &pointOnPlane, const Vector3<float> pointOnLine, const Vector3<float> &normal, const Vector3<float> &line) NOEXCEPT
	{
		return Vector3<float>::DotProduct(pointOnPlane - pointOnLine, normal) / Vector3<float>::DotProduct(line, normal) * line + pointOnLine;
	}

};