//Header file.
#include <Entities/VegetationEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
VegetationEntity::VegetationEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewVegetationComponentsIndex(this);
}

/*
*	Initializes this vegetation entity.
*/
void VegetationEntity::Initialize(const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) NOEXCEPT
{
	//Initialize this vegetation entity via the rendering system.
	RenderingSystem::Instance->InitializeVegetationEntity(*this, material, transformations, properties);
}