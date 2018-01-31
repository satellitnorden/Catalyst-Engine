//Header file.
#include <Entity.h>

//Systems.
#include <EntitySystem.h>
#include <PhysicsSystem.h>

//Static variable definitions.
DynamicArray<Entity *CATALYST_RESTRICT> Entity::instances;

/*
*	Default constructor.
*/
Entity::Entity() CATALYST_NOEXCEPT
{
	//Add this entity to the universal container.
	instances.Emplace(this);
}

/*
*	Default destructor.
*/
Entity::~Entity() CATALYST_NOEXCEPT
{
	//Remove this entity from the universal container.
	instances.Erase(this);
}

/*
*	Marks this entitiy for destruction.
*/
void Entity::MarkForDestruction() CATALYST_NOEXCEPT
{

}

/*
*	Returns the world position of this entity.
*/
Vector3 Entity::GetWorldPosition() const CATALYST_NOEXCEPT
{
	Vector3 worldPosition{ localPosition.GetSafe() };
	const Entity *CATALYST_RESTRICT parentEntity = parent;

	while (parentEntity)
	{
		worldPosition += parentEntity->GetLocalPosition();

		parentEntity = parentEntity->GetParent();
	}

	return worldPosition;
}

/*
*	Returns the world rotation of this entity.
*/
Vector3 Entity::GetWorldRotation() const CATALYST_NOEXCEPT
{
	Vector3 worldRotation{ localRotation };
	const Entity *CATALYST_RESTRICT parentEntity = parent;

	while (parentEntity)
	{
		worldRotation += parentEntity->GetLocalRotation();

		parentEntity = parentEntity->GetParent();
	}

	return worldRotation;
}

/*
*	Returns the world position of this world entity.
*/
Vector3 Entity::GetWorldScale() const CATALYST_NOEXCEPT
{
	Vector3 worldScale{ localScale };
	const Entity *CATALYST_RESTRICT parentEntity = parent;

	while (parentEntity)
	{
		worldScale *= parentEntity->GetLocalScale();

		parentEntity = parentEntity->GetParent();
	}

	return worldScale;
}

/*
*	Initializes physics for this entity.
*/
void Entity::InitializePhysics(const float initialMass, const Vector3 &initialVelocity) CATALYST_NOEXCEPT
{
	//Create the physics component.
	PhysicsComponent *CATALYST_RESTRICT newPhysicsComponent = new PhysicsComponent(this, initialMass, initialVelocity);

	//Register the physics component.
	PhysicsSystem::Instance->RegisterPhysicsComponent(newPhysicsComponent);

	//Add it to the components container.
	components.Emplace(ComponentType::PhysicsComponent, static_cast<void *CATALYST_RESTRICT>(newPhysicsComponent));
}

/*
*	Destroys this entity.
*/
void Entity::Destroy() CATALYST_NOEXCEPT
{
	//Destroy all components.
	if (PhysicsComponent *CATALYST_RESTRICT physicsComponent{ GetPhysicsComponentSafe() })
	{
		//Unregister the physics component.
		PhysicsSystem::Instance->UnregisterPhysicsComponent(physicsComponent);

		//Delete it.
		delete physicsComponent;
	}
}