//Header file.
#include <Entities/Types/StaticModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/StaticModelInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
StaticModelEntity::StaticModelEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::StaticModel;
}

/*
*	Initializes this entity.
*/
void StaticModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewStaticModelComponentsIndex(this);

	//Copy the data.
	const StaticModelInitializationData *const RESTRICT staticModelInitializationData{ static_cast<const StaticModelInitializationData *const RESTRICT>(data) };
	StaticModelComponent& staticModelComponent{ ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex] };
	TransformComponent& transformComponent{ ComponentManager::GetStaticModelTransformComponents()[_ComponentsIndex] };

	staticModelComponent._Model = staticModelInitializationData->_Model;
	staticModelComponent._Material = staticModelInitializationData->_Material;
	transformComponent._WorldTransform = staticModelInitializationData->_Transform;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<StaticModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void StaticModelEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnStaticModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the material.
*/
RESTRICTED NO_DISCARD Material *const RESTRICT StaticModelEntity::GetMaterial() NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._Material;
}