//Header file.
#include <PhysicsSystem.h>

//Components.
#include <PhysicsComponent.h>

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
	//Aply physics to all physics components.
	for (auto physicsComponent : physicsComponents)
	{
		//Load all values in the physics component.
		Entity *CATALYST_RESTRICT owner = physicsComponent->GetOwner();
		const float mass = physicsComponent->GetMass();
		Vector3 &velocity = physicsComponent->GetVelocity();

		//Apply graviation.
		
		velocity += Vector3(0.0f, -PHYSICAL_CONSTANT_GRAVITATIONAL_STRENGTH, 0.0f) * deltaTime;

		//Move the entity.
		owner->Move(velocity * deltaTime);

		//Check for collision.
		Vector3 ownerPosition = owner->GetLocalPosition();

		if (ownerPosition.Y < 0.0f)
		{
			ownerPosition.Y = 0.0f;

			owner->SetLocalPosition(ownerPosition);

			velocity.Y = 0.0f;
		}
	}
}

/*
*	Registers a physics component.
*/
void PhysicsSystem::RegisterPhysicsComponent(PhysicsComponent *CATALYST_RESTRICT newPhysicsComponent) CATALYST_NOEXCEPT
{
	physicsComponents.Emplace(newPhysicsComponent);
}

/*
*	Unregisters a physics component.
*/
void PhysicsSystem::UnregisterPhysicsComponent(PhysicsComponent *CATALYST_RESTRICT physicsComponent) CATALYST_NOEXCEPT
{
	physicsComponents.Erase(physicsComponent);
}