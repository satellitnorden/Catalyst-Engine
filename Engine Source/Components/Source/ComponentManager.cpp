//Header file.
#include <ComponentManager.h>

//Static variable definitions.
size_t ComponentManager::numberOfPhysicalEntityComponents = 0;
DynamicArray<FrustumCullingComponent> ComponentManager::physicalEntityFrustumCullingComponents;
DynamicArray<PhysicalGraphicsComponent> ComponentManager::physicalEntityGraphicsComponents;
DynamicArray<TransformComponent> ComponentManager::physicalEntityTransformComponents;

/*
*	Returns a new components index for physical entities.
*/
size_t ComponentManager::GetNewPhysicalEntityComponentsIndex() CATALYST_NOEXCEPT
{
	//Create the relevant components.
	physicalEntityFrustumCullingComponents.Emplace();
	physicalEntityGraphicsComponents.Emplace();
	physicalEntityTransformComponents.Emplace();

	//Return the new index.
	return numberOfPhysicalEntityComponents++;
}

/*
*	Returns the number of physical entity components.
*/
size_t ComponentManager::GetNumberOfPhysicalEntityComponents() CATALYST_NOEXCEPT
{
	//Return the number of physical entity components.
	return numberOfPhysicalEntityComponents;
}

/*
*	Returns physical entity frustum culling components.
*/
CATALYST_RESTRICTED FrustumCullingComponent* ComponentManager::GetPhysicalEntityFrustumCullingComponents() CATALYST_NOEXCEPT
{
	//Return physical entity frustum culling components.
	return physicalEntityFrustumCullingComponents.Data();
}

/*
*	Returns physical entity graphics components.
*/
CATALYST_RESTRICTED PhysicalGraphicsComponent* ComponentManager::GetPhysicalEntityGraphicsComponents() CATALYST_NOEXCEPT
{
	//Return physical entity graphics components.
	return physicalEntityGraphicsComponents.Data();
}

/*
*	Returns physical entity frustum transform components.
*/
CATALYST_RESTRICTED TransformComponent* ComponentManager::GetPhysicalEntityTransformComponents() CATALYST_NOEXCEPT
{
	//Return physical entity frustum transform components.
	return physicalEntityTransformComponents.Data();
}