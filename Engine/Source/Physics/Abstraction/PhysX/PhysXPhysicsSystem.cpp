#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Physics/Abstraction/PhysX/PhysXPhysicsSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PhysXPhysicsSystem);

/*
*	Initializes the PhysX physics system.
*/
void PhysXPhysicsSystem::InitializeSystem() NOEXCEPT
{
	//Create the foundation.
	_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _AllocatorCallback, _ErrorCallback);
}

/*
*	Updates the PhysX physics system synchronously.
*/
void PhysXPhysicsSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{

}

/*
*	Releases the PhysX physics system.
*/
void PhysXPhysicsSystem::ReleaseSystem() NOEXCEPT
{

}
#endif