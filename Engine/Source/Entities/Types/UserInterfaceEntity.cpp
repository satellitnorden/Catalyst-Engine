//Header file.
#include <Entities/Types/UserInterfaceEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/UserInterfaceInitializationData.h>

//Rendering.
#include <Rendering/Native/UserInterfaceUniformData.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
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
	component._WorldPosition = user_interface_initialization_data->_InitialWorldPosition;
	component._Rotation = user_interface_initialization_data->_InitialRotation;
	component._Scale = user_interface_initialization_data->_InitialScale;
	component._Roughness = user_interface_initialization_data->_InitialRoughness;
	component._Metallic = user_interface_initialization_data->_InitialMetallic;
	component._AmbientOcclusion = user_interface_initialization_data->_InitialAmbientOcclusion;
	component._EmissiveMultiplier = user_interface_initialization_data->_InitialEmissiveMultiplier;

	if (component._UserInterfaceScene)
	{
		//Tell the user interface scene that it is three dimensional.
		component._UserInterfaceScene->SetIsThreeDimensional(true);

		//Activate the user interface scene.
		UserInterfaceSystem::Instance->ActivateScene(component._UserInterfaceScene);
	}

	//Create the uniform buffers.
	component._UniformBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &uniform_buffer : component._UniformBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(UserInterfaceUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &uniform_buffer);
	}

	//Create the render data tables.
	component._RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (uint64 i{ 0 }, size{ component._RenderDataTables.Size() }; i < size; ++i)
	{
		RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::USER_INTERFACE), &component._RenderDataTables[i]);
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &component._RenderDataTables[i], component._UniformBuffers[i]);
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

	//Destroy the render data tables.
	for (RenderDataTableHandle &render_data_table : component._RenderDataTables)
	{
		RenderingSystem::Instance->DestroyRenderDataTable(&render_data_table);
	}

	//Destroy the uniform buffers.
	for (BufferHandle &uniform_buffer : component._UniformBuffers)
	{
		RenderingSystem::Instance->DestroyBuffer(&uniform_buffer);
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
	data->_InitialWorldPosition = component._WorldPosition;
	data->_InitialRotation = component._Rotation;
	data->_InitialScale = component._Scale;
	data->_InitialRoughness = component._Roughness;
	data->_InitialMetallic = component._Metallic;
	data->_InitialAmbientOcclusion = component._AmbientOcclusion;
	data->_InitialEmissiveMultiplier = component._EmissiveMultiplier;

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
*	Returns the world position.
*/
NO_DISCARD const WorldPosition &UserInterfaceEntity::GetWorldPosition() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._WorldPosition;
}

/*
*	Sets the world position.
*/
void UserInterfaceEntity::SetWorldPosition(const WorldPosition &value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._WorldPosition = value;
}

/*
*	Returns the rotation.
*/
NO_DISCARD const EulerAngles &UserInterfaceEntity::GetRotation() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Sets the rotation.
*/
void UserInterfaceEntity::SetRotation(const EulerAngles &value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Rotation = value;
}

/*
*	Returns the scale.
*/
NO_DISCARD const Vector2<float32> &UserInterfaceEntity::GetScale() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Scale;
}

/*
*	Sets the scale.
*/
void UserInterfaceEntity::SetScale(const Vector2<float32> &value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Scale = value;
}

/*
*	Returns the roughness.
*/
NO_DISCARD float32 UserInterfaceEntity::GetRoughness() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Roughness;
}

/*
*	Sets the roughness.
*/
void UserInterfaceEntity::SetRoughness(const float32 value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Roughness = value;
}

/*
*	Returns the metallic.
*/
NO_DISCARD float32 UserInterfaceEntity::GetMetallic() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Metallic;
}

/*
*	Sets the metallic.
*/
void UserInterfaceEntity::SetMetallic(const float32 value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._Metallic = value;
}

/*
*	Returns the ambient occlusion.
*/
NO_DISCARD float32 UserInterfaceEntity::GetAmbientOcclusion() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._AmbientOcclusion;
}

/*
*	Sets the ambient occlusion.
*/
void UserInterfaceEntity::SetAmbientOcclusion(const float32 value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._AmbientOcclusion = value;
}

/*
*	Returns the emissive multiplier.
*/
NO_DISCARD float32 UserInterfaceEntity::GetEmissiveMultiplier() const NOEXCEPT
{
	return ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._EmissiveMultiplier;
}

/*
*	Sets the emissive multiplier.
*/
void UserInterfaceEntity::SetEmissiveMultiplier(const float32 value) NOEXCEPT
{
	ComponentManager::GetUserInterfaceUserInterfaceComponents()[_ComponentsIndex]._EmissiveMultiplier = value;
}