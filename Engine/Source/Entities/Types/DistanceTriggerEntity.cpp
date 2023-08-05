//Header file.
#include <Entities/Types/DistanceTriggerEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/DistanceTriggerInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>

/*
*	Default constructor.
*/
DistanceTriggerEntity::DistanceTriggerEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::DistanceTrigger;
}

/*
*	Initializes this entity.
*/
void DistanceTriggerEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewDistanceTriggerComponentsIndex(this);

	//Copy the data.
	const DistanceTriggerInitializationData*const RESTRICT distance_trigger_initialization_data{ static_cast<const DistanceTriggerInitializationData*const RESTRICT>(data) };
	DistanceTriggerComponent &distance_trigger_component{ ComponentManager::GetDistanceTriggerDistanceTriggerComponents()[_ComponentsIndex] };

	distance_trigger_component._Position = distance_trigger_initialization_data->_Position;
	distance_trigger_component._TriggerDistance = distance_trigger_initialization_data->_TriggerDistance;
	distance_trigger_component._TriggerDistanceSquared = distance_trigger_component._TriggerDistance * distance_trigger_component._TriggerDistance;
	distance_trigger_component._EnterFunction = distance_trigger_initialization_data->_EnterFunction;
	distance_trigger_component._ExitFunction = distance_trigger_initialization_data->_ExitFunction;
	distance_trigger_component._CurrentState = State::UNENTERED;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<DistanceTriggerInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void DistanceTriggerEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDistanceTriggerComponentsIndex(_ComponentsIndex);
}