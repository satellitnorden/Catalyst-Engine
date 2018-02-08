//Header file.
#include <PhysicsSystem.h>

//Entities.
#include <Entity.h>

//Physics.
#include <PhysicalConstants.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Default constructor.
*/
PhysicsSystem::PhysicsSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
PhysicsSystem::~PhysicsSystem() CATALYST_NOEXCEPT
{

}

/*
*	Updates the physics system synchronously.
*/
void PhysicsSystem::UpdateSystemSynchronous(const float deltaTime) CATALYST_NOEXCEPT
{
	
}