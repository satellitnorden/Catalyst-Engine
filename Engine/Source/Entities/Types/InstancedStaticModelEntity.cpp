//Header file.
#include <Entities/Types/InstancedStaticModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/InstancedStaticModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/PhysicsSystem.h>

/*
*	Default constructor.
*/
InstancedStaticModelEntity::InstancedStaticModelEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::InstancedStaticModel;
}

/*
*	Initializes this entity.
*/
void InstancedStaticModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewInstancedStaticModelComponentsIndex(this);

	//Copy the data.
	const InstancedStaticModelInitializationData *const RESTRICT model_initialization_data{ static_cast<const InstancedStaticModelInitializationData *const RESTRICT>(data) };
	InstancedStaticModelComponent &component{ ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents()[_ComponentsIndex] };

	ASSERT(!model_initialization_data->_WorldTransforms.Empty(), "Trying to add instanced static model entity without any transforms!");

	component._ModelResource = model_initialization_data->_ModelResource;
	component._MaterialResources = model_initialization_data->_MaterialResources;

	Vector3<float32> average_cell{ 0.0f, 0.0f, 0.0f };

	for (const WorldTransform &world_transform : model_initialization_data->_WorldTransforms)
	{
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			average_cell[i] += static_cast<float32>(world_transform.GetCell()[i]);
		}
	}

	average_cell /= static_cast<float32>(model_initialization_data->_WorldTransforms.Size());

	for (uint8 i{ 0 }; i < 3; ++i)
	{
		component._Cell[i] = CatalystBaseMath::Round<int32>(average_cell[i]);
	}

	DynamicArray<Matrix4x4> transformations;
	transformations.Reserve(model_initialization_data->_WorldTransforms.Size());

	for (const WorldTransform &world_transform : model_initialization_data->_WorldTransforms)
	{
		transformations.Emplace(world_transform.ToRelativeMatrix4x4(component._Cell));
	}

	RenderingUtilities::CreateTransformationsBuffer(transformations, &component._TransformationsBuffer);

	component._NumberOfTransformations = static_cast<uint32>(transformations.Size());
	component._ModelFlags = model_initialization_data->_ModelFlags;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<InstancedStaticModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void InstancedStaticModelEntity::Terminate() NOEXCEPT
{
	//Destroy the transformations buffer.
	RenderingSystem::Instance->DestroyBuffer(&ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents()[_ComponentsIndex]._TransformationsBuffer);

	//Return this entitiy's components index.
	ComponentManager::ReturnInstancedStaticModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the model resources.
*/
NO_DISCARD ResourcePointer<ModelResource> InstancedStaticModelEntity::GetModelResource() const NOEXCEPT
{
	return ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents()[_ComponentsIndex]._ModelResource;
}

/*
*	Returns the material resources.
*/
NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &InstancedStaticModelEntity::GetMaterialResources() const NOEXCEPT
{
	return ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents()[_ComponentsIndex]._MaterialResources;
}