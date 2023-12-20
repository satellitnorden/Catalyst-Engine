//Header file.
#include <Entities/Types/GrassEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/GrassInitializationData.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/PhysicsSystem.h>

/*
*	Default constructor.
*/
GrassEntity::GrassEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Grass;
}

/*
*	Initializes this entity.
*/
void GrassEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewGrassComponentsIndex(this);

	//Copy the data.
	const GrassInitializationData *const RESTRICT type_initialization_data{ static_cast<const GrassInitializationData*const RESTRICT>(data) };
	GrassComponent &component{ ComponentManager::GetGrassGrassComponents()[_ComponentsIndex] };

	ASSERT(!type_initialization_data->_WorldPositions.Empty(), "Trying to add grass entity without any positions!");

	//Calculate the cell.
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

	//Calculate the axis aligned bounding box.
	component._WorldSpaceAxisAlignedBoundingBox._Minimum.SetCell(component._Cell);
	component._WorldSpaceAxisAlignedBoundingBox._Maximum.SetCell(component._Cell);

	Vector3<float32> minimum{ FLOAT32_MAXIMUM };
	Vector3<float32> maximum{ -FLOAT32_MAXIMUM };

	for (const WorldPosition &world_position : type_initialization_data->_WorldPositions)
	{
		const Vector3<float32> relative_position{ world_position.GetRelativePosition(component._Cell) };

		minimum = Vector3<float32>::Minimum(minimum, relative_position);
		maximum = Vector3<float32>::Maximum(maximum, relative_position);
	}

	component._WorldSpaceAxisAlignedBoundingBox._Minimum.SetLocalPosition(minimum);
	component._WorldSpaceAxisAlignedBoundingBox._Maximum.SetLocalPosition(maximum);

	struct GrassInstance final
	{
		Vector3<float32> _Position;
		uint32 _Seed;
	};

	DynamicArray<GrassInstance> instances;
	instances.Reserve(type_initialization_data->_WorldPositions.Size());

	for (const WorldPosition &world_position : type_initialization_data->_WorldPositions)
	{
		instances.Emplace();
		GrassInstance &new_grass_instance{ instances.Back() };

		new_grass_instance._Position = world_position.GetRelativePosition(component._Cell);
		new_grass_instance._Seed = CatalystRandomMath::RandomIntegerInRange<uint32>(0, UINT32_MAXIMUM);
	}

	const void *RESTRICT positions_data[]{ instances.Data() };
	const uint64 positions_data_sizes[]{ sizeof(GrassInstance) * instances.Size() };
	RenderingSystem::Instance->CreateBuffer(positions_data_sizes[0], BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &component._InstanceBuffer);
	RenderingSystem::Instance->UploadDataToBuffer(positions_data, positions_data_sizes, 1, &component._InstanceBuffer);

	component._NumberOfInstances = static_cast<uint32>(instances.Size());

	//Copy other properties.
	component._MaterialResource = type_initialization_data->_MaterialResource;
	component._Thickness = type_initialization_data->_Thickness;
	component._Height = type_initialization_data->_Height;
	component._Tilt = type_initialization_data->_Tilt;
	component._Bend = type_initialization_data->_Bend;
	component._FadeOutDistance = type_initialization_data->_FadeOutDistance;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<GrassInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void GrassEntity::Terminate() NOEXCEPT
{
	//Destroy the instance buffer.
	RenderingSystem::Instance->DestroyBuffer(&ComponentManager::GetGrassGrassComponents()[_ComponentsIndex]._InstanceBuffer);

	//Return this entitiy's components index.
	ComponentManager::ReturnGrassComponentsIndex(_ComponentsIndex);
}