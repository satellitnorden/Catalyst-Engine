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
*	Returns the preprocessing parameters.
*/
void InstancedStaticModelEntity::GetPreprocessingParameters(EntityPreprocessingParameters *const RESTRICT parameters) NOEXCEPT
{
	parameters->_ShouldPreprocess = true;
	parameters->_CanPreprocessOnMainThread = false;
}

/*
*	Preprocesses this entity.
*/
void InstancedStaticModelEntity::Preprocess(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Cast to instanced static model initialization data.
	InstancedStaticModelInitializationData *const RESTRICT instanced_static_model_initialization_data{ static_cast<InstancedStaticModelInitializationData* const RESTRICT>(data) };

	ASSERT(!instanced_static_model_initialization_data->_WorldTransforms.Empty(), "Trying to add instanced static model entity without any transforms!");

	//Calculate the average cell for all transforms.
	Vector3<float32> average_cell{ 0.0f, 0.0f, 0.0f };

	for (const WorldTransform& world_transform : instanced_static_model_initialization_data->_WorldTransforms)
	{
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			average_cell[i] += static_cast<float32>(world_transform.GetCell()[i]);
		}
	}

	average_cell /= static_cast<float32>(instanced_static_model_initialization_data->_WorldTransforms.Size());

	for (uint8 i{ 0 }; i < 3; ++i)
	{
		instanced_static_model_initialization_data->_PreprocessedData._Cell[i] = CatalystBaseMath::Round<int32>(average_cell[i]);
	}

	//Create the transoformations buffer. Calculate the world space axis aligned bounding box in the process.
	DynamicArray<Matrix4x4> transformations;
	transformations.Reserve(instanced_static_model_initialization_data->_WorldTransforms.Size());

	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	for (const WorldTransform& world_transform : instanced_static_model_initialization_data->_WorldTransforms)
	{
		const Matrix4x4 transformation{ world_transform.ToRelativeMatrix4x4(instanced_static_model_initialization_data->_PreprocessedData._Cell) };

		transformations.Emplace(transformation);

		AxisAlignedBoundingBox3D transformation_axis_aligned_bounding_box;
		RenderingUtilities::TransformAxisAlignedBoundingBox(instanced_static_model_initialization_data->_ModelResource->_ModelSpaceAxisAlignedBoundingBox, transformation, &transformation_axis_aligned_bounding_box);
	
		axis_aligned_bounding_box.Expand(transformation_axis_aligned_bounding_box);
	}

	RenderingUtilities::CreateTransformationsBuffer(transformations, &instanced_static_model_initialization_data->_PreprocessedData._TransformationsBuffer);

	//Set up the world space axis aligned bounding box.
	instanced_static_model_initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox = WorldSpaceAxisAlignedBoundingBox3D
	(
		WorldPosition(instanced_static_model_initialization_data->_PreprocessedData._Cell, axis_aligned_bounding_box._Minimum),
		WorldPosition(instanced_static_model_initialization_data->_PreprocessedData._Cell, axis_aligned_bounding_box._Maximum)
	);

	//Set the number of transformations.
	instanced_static_model_initialization_data->_PreprocessedData._NumberOfTransformations = static_cast<uint32>(transformations.Size());
}

/*
*	Initializes this entity.
*/
void InstancedStaticModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewInstancedStaticModelComponentsIndex(this);

	//Copy the data.
	const InstancedStaticModelInitializationData *const RESTRICT instanced_static_model_initialization_data{ static_cast<const InstancedStaticModelInitializationData *const RESTRICT>(data) };
	InstancedStaticModelComponent &component{ ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents()[_ComponentsIndex] };

	component._WorldSpaceAxisAlignedBoundingBox = instanced_static_model_initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox;
	component._ModelResource = instanced_static_model_initialization_data->_ModelResource;
	component._MaterialResources = instanced_static_model_initialization_data->_MaterialResources;
	component._Cell = instanced_static_model_initialization_data->_PreprocessedData._Cell;
	component._TransformationsBuffer = instanced_static_model_initialization_data->_PreprocessedData._TransformationsBuffer;
	component._NumberOfTransformations = instanced_static_model_initialization_data->_PreprocessedData._NumberOfTransformations;
	component._ModelFlags = instanced_static_model_initialization_data->_ModelFlags;
	component._ModelFadeData = instanced_static_model_initialization_data->_ModelFadeData;

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