#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Physics.
#include <Physics/Ray.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

class CatalystVectorMath final
{

public:

	/*
	*	Performs a line-box intersection and returns whether or not there was an intersection.
	*/
	constexpr static NO_DISCARD bool LineBoxIntersection(const AxisAlignedBoundingBox &box, const Ray &ray) NOEXCEPT
	{
		//Pre-calculate the inverse direction of the ray to avoid costly divisions.
		const Vector3 inverseDirection{ Vector3(1.0f) / ray._Direction };

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
	constexpr static NO_DISCARD Vector3 LinePlaneIntersection(const Vector3 &pointOnPlane, const Vector3 pointOnLine, const Vector3 &normal, const Vector3 &line) NOEXCEPT
	{
		return Vector3::DotProduct(pointOnPlane - pointOnLine, normal) / Vector3::DotProduct(line, normal) * line + pointOnLine;
	}

};