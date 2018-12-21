#if defined(CATALYST_PHYSICS_PHYSX)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysxAllocatorCallback.h>
#include <Physics/Abstraction/PhysX/PhysxErrorCallback.h>
#include <Physics/Abstraction/PhysX/PhysxCore.h>

class PhysXPhysicsSystem final
{

public:

	//Singelton declaration.
	DECLARE_SINGLETON(PhysXPhysicsSystem);

	/*
	*	Default constructor.
	*/
	PhysXPhysicsSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the PhysX physics system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the PhysX physics system synchronously.
	*/
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Releases the PhysX physics system.
	*/
	void ReleaseSystem() NOEXCEPT;

private:

	//The foundation object.
	physx::PxFoundation *RESTRICT _Foundation;

	//The allocator callback.
	PhysxAllocatorCallback _AllocatorCallback;

	//The error callback.
	PhysxErrorCallback _ErrorCallback;

};
#endif