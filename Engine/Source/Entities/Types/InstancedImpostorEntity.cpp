//Header file.
#include <Entities/Types/InstancedImpostorEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/InstancedImpostorInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/PhysicsSystem.h>

/*
*	Default constructor.
*/
InstancedImpostorEntity::InstancedImpostorEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::InstancedImpostor;
}

/*
*	Initializes this entity.
*/
void InstancedImpostorEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewInstancedImpostorComponentsIndex(this);

	//Copy the data.
	const InstancedImpostorInitializationData *const RESTRICT type_initialization_data{ static_cast<const InstancedImpostorInitializationData *const RESTRICT>(data) };
	InstancedImpostorComponent &component{ ComponentManager::GetInstancedImpostorInstancedImpostorComponents()[_ComponentsIndex] };

	ASSERT(!type_initialization_data->_WorldPositions.Empty(), "Trying to add instanced impostor entity without any positions!");

	component._MaterialResource = type_initialization_data->_MaterialResource;
	component._Dimensions = type_initialization_data->_Dimensions;
	component._StartFadeInDistance = type_initialization_data->_StartFadeInDistance;
	component._EndFadeInDistance = type_initialization_data->_EndFadeInDistance;
	component._StartFadeOutDistance = type_initialization_data->_StartFadeOutDistance;
	component._EndFadeOutDistance = type_initialization_data->_EndFadeOutDistance;

	Vector3<float32> average_cell{ 0.0f, 0.0f, 0.0f };

	for (const WorldPosition &world_position : type_initialization_data->_WorldPositions)
	{
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			average_cell[i] += static_cast<float32>(world_position.GetCell()[i]);
		}
	}

	average_cell /= static_cast<float32>(type_initialization_data->_WorldPositions.Size());

	for (uint8 i{ 0 }; i < 3; ++i)
	{
		component._Cell[i] = CatalystBaseMath::Round<int32>(average_cell[i]);
	}

	DynamicArray<Vector3<float32>> transformations;
	transformations.Reserve(type_initialization_data->_WorldPositions.Size());

	for (const WorldPosition &world_position : type_initialization_data->_WorldPositions)
	{
		transformations.Emplace(world_position.GetRelativePosition(component._Cell));
	}

	const void *RESTRICT transformation_data[]{ transformations.Data() };
	const uint64 transformation_data_sizes[]{ sizeof(Vector3<float32>) * transformations.Size() };
	RenderingSystem::Instance->CreateBuffer(transformation_data_sizes[0], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &component._TransformationsBuffer);
	RenderingSystem::Instance->UploadDataToBuffer(transformation_data, transformation_data_sizes, 1, &component._TransformationsBuffer);

	component._NumberOfTransformations = static_cast<uint32>(transformations.Size());

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<InstancedImpostorInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void InstancedImpostorEntity::Terminate() NOEXCEPT
{
	//Destroy the transformations buffer.
	RenderingSystem::Instance->DestroyBuffer(&ComponentManager::GetInstancedImpostorInstancedImpostorComponents()[_ComponentsIndex]._TransformationsBuffer);

	//Return this entitiy's components index.
	ComponentManager::ReturnInstancedImpostorComponentsIndex(_ComponentsIndex);
}