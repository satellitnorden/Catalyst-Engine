//Header file.
#include <Entities/Types/UserInterfaceEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/UserInterfaceInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/UserInterfaceSystem.h>

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

	component._UserInterfaceScene = UserInterfaceSystem::Instance->CreateUserInterfaceScene(user_interface_initialization_data->_UserInterfaceSceneIdentifier);
	component._WorldTransform = user_interface_initialization_data->_InitialWorldTransform;

	if (component._UserInterfaceScene)
	{
		//Tell the user interface scene that it is three dimensional.
		component._UserInterfaceScene->SetIsThreeDimensional(true);

		//Activate the user interface scene.
		UserInterfaceSystem::Instance->ActivateScene(component._UserInterfaceScene);
	}

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

	if (component._UserInterfaceScene)
	{
		//Deactivate the user interface scene.
		UserInterfaceSystem::Instance->DeactivateScene(component._UserInterfaceScene);
	}

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
	data->_UserInterfaceSceneIdentifier = component._UserInterfaceScene->GetIdentifier();
	data->_InitialWorldTransform = component._WorldTransform;

	//Return the initialization data.
	return data;
}

/*
*	Returns the user interface scene.
*/
RESTRICTED NO_DISCARD const UserInterfaceScene *const RESTRICT UserInterfaceEntity::GetUserInterfaceScene() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._UserInterfaceScene;
}

/*
*	Sets the user interface scene.
*/
void UserInterfaceEntity::SetUserInterfaceScene(UserInterfaceScene *const RESTRICT value) NOEXCEPT
{
	//Deactivate/destroy the old user interface scene, if there is one.
	if (ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._UserInterfaceScene)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfaceScene(ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._UserInterfaceScene);
	}

	//Set the user interface scene.
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._UserInterfaceScene = value;

	//Tell the user interface scene that it is three dimensional.
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._UserInterfaceScene->SetIsThreeDimensional(true);

	//Activate the new user interface scene.
	UserInterfaceSystem::Instance->ActivateScene(ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._UserInterfaceScene);
}

/*
*	Returns the world transform.
*/
NO_DISCARD const WorldTransform &UserInterfaceEntity::GetWorldTransform() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._WorldTransform;
}

/*
*	Sets the world transform.
*/
void UserInterfaceEntity::SetWorldTransform(const WorldTransform &value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._WorldTransform = value;
}