#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

class CatalystVectorMath final
{

public:

	/*
	*	Performs a line-plane intersection and returns the intersection point.
	*/
	NO_DISCARD constexpr static Vector3 LinePlaneIntersection(const Vector3 &pointOnPlane, const Vector3 pointOnLine, const Vector3 &normal, const Vector3 &line) NOEXCEPT
	{
		return Vector3::DotProduct(pointOnPlane - pointOnLine, normal) / Vector3::DotProduct(line, normal) * line + pointOnLine;
	}

};