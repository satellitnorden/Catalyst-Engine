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
PhysicsSystem::PhysicsSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
PhysicsSystem::~PhysicsSystem() NOEXCEPT
{

}

/*
*	Updates the physics system synchronously.
*/
void PhysicsSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	
}