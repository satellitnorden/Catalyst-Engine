#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysxCore.h>

namespace PhysXAbstractionUtilities
{

	/*
	*	Converts a PxVec3 to a Vector3<float>.
	*/
	static Vector3<float> ToCatalystVector(const physx::PxVec3 &vector) NOEXCEPT
	{
		return Vector3<float>(vector.x, vector.y, vector.z);
	}

	/*
	*	Converts a Vector3<float> to a PxVec3.
	*/
	static physx::PxVec3 ToPhysXVector(const Vector3<float> &vector) NOEXCEPT
	{
		return physx::PxVec3(vector._X, vector._Y, vector._Z);
	}
}