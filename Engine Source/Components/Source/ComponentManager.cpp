//Header file.
#include <ComponentManager.h>

//Static variable definitions.
DynamicArray<PhysicalGraphicsComponent> ComponentManager::physicalGraphicsComponents;

/*
*	Returns the physical graphics components.
*/
const DynamicArray<PhysicalGraphicsComponent>& ComponentManager::GetPhysicalGraphicsComponents() CATALYST_NOEXCEPT
{
	//Return the physical graphics components.
	return physicalGraphicsComponents;
}

/*
*	Returns the index for a new physical entity graphics component.
*/
size_t ComponentManager::GetNewPhysicalGraphicsComponent() CATALYST_NOEXCEPT
{
	//Emplace a new physical graphics component.
	physicalGraphicsComponents.Emplace();

	//Return the new index.
	return physicalGraphicsComponents.Size() - 1;
}

/*
*	Returns the physical entity graphics component at the specified index.
*/
const PhysicalGraphicsComponent& ComponentManager::GetPhysicalGraphicsComponentConst(const size_t index) CATALYST_NOEXCEPT
{
	//Return the physical entity graphics component at the specified index.
	return physicalGraphicsComponents[index];
}

/*
*	Returns the physical entity graphics component at the specified index, non-const.
*/
PhysicalGraphicsComponent& ComponentManager::GetPhysicalGraphicsComponentNonConst(const size_t index) CATALYST_NOEXCEPT
{
	//Return the physical entity graphics component at the specified index.
	return physicalGraphicsComponents[index];
}