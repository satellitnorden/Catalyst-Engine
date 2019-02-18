//Header file.
#include <Entities/Types/PhysicsEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Creation/PhysicsInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
PhysicsEntity::PhysicsEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Physics;
}

/*
*	Initializes this entity.
*/
void PhysicsEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewPhysicsComponentsIndex(this);

	//Copy the data over to the components.
	PhysicsComponent &physicsComponent{ ComponentManager::GetPhysicsPhysicsComponents()[_ComponentsIndex] };
	TransformComponent &transformComponent{ ComponentManager::GetPhysicsTransformComponents()[_ComponentsIndex] };
	const PhysicsInitializationData *const RESTRICT physicsInitializationData{ static_cast<const PhysicsInitializationData *const RESTRICT>(data) };

	physicsComponent = physicsInitializationData->_PhysicsComponent;
	transformComponent = physicsInitializationData->_TransformComponent;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<PhysicsInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void PhysicsEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnPhysicsComponentsIndex(_ComponentsIndex);
}