//Header file.
#include <Entities/Types/VegetationEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/VegetationInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
VegetationEntity::VegetationEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Vegetation;
}

/*
*	Initializes this entity.
*/
void VegetationEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewVegetationComponentsIndex(this);

	//Copy the data.
	const VegetationInitializationData *const RESTRICT vegetationInitializationData{ static_cast<const VegetationInitializationData *const RESTRICT>(data) };
	VegetationComponent& component{ ComponentManager::GetVegetationVegetationComponents()[_ComponentsIndex] };

	RenderingUtilities::CalculateAxisAlignedBoundingBoxFromTransformations(vegetationInitializationData->_Transformations, vegetationInitializationData->_Model->_ModelSpaceAxisAlignedBoundingBox, &component._WorldSpaceAxisAlignedBoundingBox);
	component._Visibility = false;
	component._Model = vegetationInitializationData->_Model;
	component._Material = vegetationInitializationData->_Material;
	component._MaskTextureIndex = vegetationInitializationData->_MaskTextureIndex;
	component._CutoffDistance = vegetationInitializationData->_CutoffDistance;
	component._LargeScaleWindDisplacementFactor = vegetationInitializationData->_LargeScaleWindDisplacementFactor;
	component._MediumScaleWindDisplacementFactor = vegetationInitializationData->_MediumScaleWindDisplacementFactor;
	component._SmallScaleWindDisplacementFactor = vegetationInitializationData->_SmallScaleWindDisplacementFactor;
	RenderingUtilities::CreateTransformationsBuffer(vegetationInitializationData->_Transformations, &component._TransformationsBuffer);
	component._NumberOfTransformations = static_cast<uint32>(vegetationInitializationData->_Transformations.Size());

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<VegetationInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void VegetationEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnVegetationComponentsIndex(_ComponentsIndex);
}