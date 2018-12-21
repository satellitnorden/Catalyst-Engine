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

	//Create the tolerances scale.
	physx::PxTolerancesScale tolerancesScale;

	tolerancesScale.length = 100.0f;
	tolerancesScale.speed = 981.0f;

	//Create the physics.
	_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_Foundation, tolerancesScale, false, nullptr);

	//Create the scene description.
	physx::PxSceneDesc sceneDescription{ tolerancesScale };

	//Create the scene.
	_Scene = _Physics->createScene(sceneDescription);
}

/*
*	Updates the PhysX physics system synchronously.
*/
void PhysXPhysicsSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Simulate the scene.
	//_Scene->simulate(context->_DeltaTime);

	//Fetch the results.
	//_Scene->fetchResults(true);
}

/*
*	Releases the PhysX physics system.
*/
void PhysXPhysicsSystem::ReleaseSystem() NOEXCEPT
{
	//Release the scene.
	//_Scene->release();

	//Release the physics.
	_Physics->release();

	//Release the foundation.
	_Foundation->release();
}
#endif