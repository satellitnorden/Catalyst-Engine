//Header file.
#include <Entities/Types/LightEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/LightInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
LightEntity::LightEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Light;
}

/*
*	Initializes this entity.
*/
void LightEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewLightComponentsIndex(this);

	//Copy the data over to the components.
	LightComponent &lightComponent{ ComponentManager::GetLightLightComponents()[_ComponentsIndex] };
	const LightInitializationData *const RESTRICT lightInitializationData{ static_cast<const LightInitializationData *const RESTRICT>(data) };

	lightComponent._Color = lightInitializationData->_Color;
	lightComponent._Position = lightInitializationData->_Position;
	lightComponent._AttenuationDistance = lightInitializationData->_AttenuationDistance;
	lightComponent._Size = lightInitializationData->_Size;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<LightInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void LightEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnLightComponentsIndex(_ComponentsIndex);
}