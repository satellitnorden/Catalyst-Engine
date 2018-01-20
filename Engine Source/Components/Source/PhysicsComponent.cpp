//Header file.
#include <PhysicsComponent.h>

/*
*	Default constructor.
*/
PhysicsComponent::PhysicsComponent() CATALYST_NOEXCEPT
{

}

/*
*	Constructor taking all values as arguments.
*/
PhysicsComponent::PhysicsComponent(Entity *CATALYST_RESTRICT initialOwner, const float initialMass, const Vector3 &initialVelocity) CATALYST_NOEXCEPT
	:
	Component(ComponentType::PhysicsComponent, initialOwner),
	mass(initialMass),
	velocity(initialVelocity)
{

}

/*
*	Default destructor.
*/
PhysicsComponent::~PhysicsComponent() CATALYST_NOEXCEPT
{

}