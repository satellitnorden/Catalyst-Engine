//Header file.
#include <Components/Components/UserInterfaceComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//User interface.
#include <UserInterface/UserInterfaceSceneFactory.h>

/*
*	User interface background push constant data class definition.
*/
class UserInterfaceBackgroundPushConstantData final
{

public:

	//The transformation.
	Matrix4x4 _Transformation;

};

/*
*	Initializes this component.
*/
void UserInterfaceComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableEnumerationField
	(
		"Identifier",
		offsetof(UserInterfaceInitializationData, _Identifier),
		offsetof(UserInterfaceInstanceData, _Identifier)
	);

	//Register the "UserInterface3DBackground" input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("UserInterface3DBackground"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(UserInterfaceBackgroundPushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			UserInterfaceComponent::Instance->GatherUserInterface3DBackgroundInputStream(input_stream);
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);
}

/*
*	Creates an instance.
*/
void UserInterfaceComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	UserInterfaceInitializationData *const RESTRICT _initialization_data{ static_cast<UserInterfaceInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	UserInterfaceInstanceData &instance_data{ _InstanceData.Back() };

	//Copy data.
	instance_data._Identifier = _initialization_data->_Identifier;

	//Create the scene.
	if (instance_data._Identifier != UserInterfaceSceneIdentifier::None)
	{
		instance_data._Scene = UserInterfaceSceneFactory::Create(instance_data._Identifier);
		instance_data._Scene->SetEntity(entity);
		instance_data._Scene->OnActivated();
	}

	else
	{
		instance_data._Scene = nullptr;
	}
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void UserInterfaceComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Animated model component needs a world transform component!");
}

/*
*	Destroys an instance.
*/
void UserInterfaceComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Gathers the user interface background input stream.
*/
void UserInterfaceComponent::GatherUserInterface3DBackgroundInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Go through all instances.
	for (uint64 instance_index{ 0 }; instance_index < NumberOfInstances(); ++instance_index)
	{
		//Cache data.
		Entity *const RESTRICT entity{ InstanceToEntity(instance_index) };
		const UserInterfaceInstanceData &circle_marker_instance_data{ InstanceData(entity) };
		const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

		//Add a new entry.
		input_stream->_Entries.Emplace();
		RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

		new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
		new_entry._VertexBuffer = EMPTY_HANDLE;
		new_entry._IndexBuffer = EMPTY_HANDLE;
		new_entry._IndexBufferOffset = 0;
		new_entry._InstanceBuffer = EMPTY_HANDLE;
		new_entry._VertexCount = 4;
		new_entry._IndexCount = 0;
		new_entry._InstanceCount = 0;

		//Set up the push constant data.
		UserInterfaceBackgroundPushConstantData push_constant_data;

		push_constant_data._Transformation = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

		for (uint64 i{ 0 }; i < sizeof(UserInterfaceBackgroundPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
		}
	}
}