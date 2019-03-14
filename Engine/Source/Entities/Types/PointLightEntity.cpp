//Header file.
#include <Entities/Types/PointLightEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/PointLightInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
PointLightEntity::PointLightEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::PointLight;
}

/*
*	Initializes this entity.
*/
void PointLightEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this point light entity.
	_ComponentsIndex = ComponentManager::GetNewPointLightComponentsIndex(this);

	//Copy the data over to the component.
	PointLightComponent &component{ ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex] };
	const PointLightInitializationData *const RESTRICT pointLightInitializationData{ static_cast<const PointLightInitializationData *const RESTRICT>(data) };

	component = pointLightInitializationData->_Component;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<PointLightInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void PointLightEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnPointLightComponentsIndex(_ComponentsIndex);
}