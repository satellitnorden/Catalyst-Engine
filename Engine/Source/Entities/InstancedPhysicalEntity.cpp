//Header file.
#include <Entities/InstancedPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
InstancedPhysicalEntity::InstancedPhysicalEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewInstancedPhysicalComponentsIndex(this);
}

/*
*	Initializes this instanced physical entity.
*/
void InstancedPhysicalEntity::Initialize(const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) NOEXCEPT
{
	//Initialize this instanced physical entity via the rendering system.
	RenderingSystem::Instance->InitializeInstancedPhysicalEntity(this, model, transformations);
}