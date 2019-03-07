//Header file.
#include <Entities/Types/CollisionEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Creation/CollisionInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
CollisionEntity::CollisionEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Collision;
}

/*
*	Initializes this entity.
*/
void CollisionEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewCollisionComponentsIndex(this);

	//Copy the data over to the components.
	TransformComponent &transformComponent{ ComponentManager::GetCollisionTransformComponents()[_ComponentsIndex] };
	const CollisionInitializationData *const RESTRICT collisionInitializationData{ static_cast<const CollisionInitializationData *const RESTRICT>(data) };

	transformComponent = collisionInitializationData->_TransformComponent;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<CollisionInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void CollisionEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnCollisionComponentsIndex(_ComponentsIndex);
}