#if defined(CATALYST_PHYSICS_PHYSX)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Pointers/UniquePointer.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysxAllocatorCallback.h>
#include <Physics/Abstraction/PhysX/PhysxErrorCallback.h>
#include <Physics/Abstraction/PhysX/PhysxCore.h>
#include <Physics/Abstraction/PhysX/PhysXCPUDispatcher.h>

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

	//The CPU dispatcher.
	PhysXCPUDispatcher _CPUDistpatcher;

	//The allocator callback.
	PhysxAllocatorCallback _AllocatorCallback;

	//The error callback.
	PhysxErrorCallback _ErrorCallback;

	//The foundation object.
	physx::PxFoundation *RESTRICT _Foundation;

	//The physics object.
	physx::PxPhysics *RESTRICT _Physics;

	//The scene object.
	physx::PxScene *RESTRICT _Scene;

	/*
	*	Creates the scene description.
	*/
	void CreateSceneDescription(physx::PxSceneDesc *const RESTRICT sceneDescription) NOEXCEPT;

};
#endif