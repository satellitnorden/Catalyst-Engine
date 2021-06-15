//Header file.
#include <Entities/Types/UserInterfaceEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/UserInterfaceInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>

/*
*	Default constructor.
*/
UserInterfaceEntity::UserInterfaceEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::UserInterface;
}

/*
*	Initializes this entity.
*/
void UserInterfaceEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewUserInterfaceComponentsIndex(this);

	//Copy the data.
	const UserInterfaceInitializationData *const RESTRICT user_interface_initialization_data{ static_cast<const UserInterfaceInitializationData *const RESTRICT>(data) };
	UserInterfaceComponent& component{ ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex] };

	component._UserInterfaceScene = user_interface_initialization_data->_UserInterfaceScene;
	component._WorldPosition = user_interface_initialization_data->_InitialWorldPosition;
	component._Rotation = user_interface_initialization_data->_InitialRotation;
	component._Scale = user_interface_initialization_data->_InitialScale;

	//Tell the user interface scene that it is three dimensional.
	component._UserInterfaceScene->SetIsThreeDimensional(true);

	//Activate the user interface scene.
	component._UserInterfaceScene->SetIsActive(true);
	component._UserInterfaceScene->OnActivated();

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<UserInterfaceInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void UserInterfaceEntity::Terminate() NOEXCEPT
{
	//Cache the component.
	UserInterfaceComponent& component{ ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex] };

	//Deactivate the user interface scene.
	component._UserInterfaceScene->SetIsActive(false);
	component._UserInterfaceScene->OnDeactivated();

	//Return this entitiy's components index.
	ComponentManager::ReturnUserInterfaceComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the initialization data required to duplicate this entity.
*/
RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT UserInterfaceEntity::GetDuplicationInitializationData() const NOEXCEPT
{
	//Cache the component.
	const UserInterfaceComponent& component{ ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex] };

	//Create the initialization data.
	UserInterfaceInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<UserInterfaceInitializationData>() };

	//Set up the initialization data.
	data->_UserInterfaceScene = component._UserInterfaceScene;
	data->_InitialWorldPosition = component._WorldPosition;
	data->_InitialRotation = component._Rotation;
	data->_InitialScale = component._Scale;

	//Return the initialization data.
	return data;
}