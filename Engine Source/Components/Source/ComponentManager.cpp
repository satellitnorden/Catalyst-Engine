//Header file.
#include <ComponentManager.h>

//Static variable definitions.
size_t ComponentManager::numberOfCameraEntityComponents = 0;
DynamicArray<CameraComponent> ComponentManager::cameraEntityCameraComponents;

size_t ComponentManager::numberOfPhysicalEntityComponents = 0;
DynamicArray<FrustumCullingComponent> ComponentManager::physicalEntityFrustumCullingComponents;
DynamicArray<RenderComponent> ComponentManager::physicalEntityRenderComponents;
DynamicArray<TransformComponent> ComponentManager::physicalEntityTransformComponents;

size_t ComponentManager::numberOfPointLightEntityComponents = 0;
DynamicArray<PointLightComponent> ComponentManager::pointLightEntityPointLightComponents;

size_t ComponentManager::numberOfSpotLightEntityComponents = 0;
DynamicArray<SpotLightComponent> ComponentManager::spotLightEntitySpotLightComponents;

/*
*	Returns a new components index for camera entities.
*/
size_t ComponentManager::GetNewCameraEntityComponentsIndex() CATALYST_NOEXCEPT
{
	//Create the relevant components.
	cameraEntityCameraComponents.Emplace();

	//Return the new index.
	return numberOfCameraEntityComponents++;
}

/*
*	Returns the number of camera entity components.
*/
size_t ComponentManager::GetNumberOfCameraEntityComponents() CATALYST_NOEXCEPT
{
	//Return the number of camera entity components.
	return numberOfCameraEntityComponents;
}

/*
*	Returns the camera entity camera components.
*/
CATALYST_RESTRICTED CameraComponent* ComponentManager::GetCameraEntityCameraComponents() CATALYST_NOEXCEPT
{
	//Return the camera entity camera components.
	return cameraEntityCameraComponents.Data();
}

/*
*	Returns a new components index for physical entities.
*/
size_t ComponentManager::GetNewPhysicalEntityComponentsIndex() CATALYST_NOEXCEPT
{
	//Create the relevant components.
	physicalEntityFrustumCullingComponents.Emplace();
	physicalEntityRenderComponents.Emplace();
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
*	Returns the physical entity frustum culling components.
*/
CATALYST_RESTRICTED FrustumCullingComponent* ComponentManager::GetPhysicalEntityFrustumCullingComponents() CATALYST_NOEXCEPT
{
	//Return the physical entity frustum culling components.
	return physicalEntityFrustumCullingComponents.Data();
}

/*
*	Returns the physical entity render components.
*/
CATALYST_RESTRICTED RenderComponent* ComponentManager::GetPhysicalEntityRenderComponents() CATALYST_NOEXCEPT
{
	//Return the physical entity render components.
	return physicalEntityRenderComponents.Data();
}

/*
*	Returns the physical entity frustum transform components.
*/
CATALYST_RESTRICTED TransformComponent* ComponentManager::GetPhysicalEntityTransformComponents() CATALYST_NOEXCEPT
{
	//Return the physical entity frustum transform components.
	return physicalEntityTransformComponents.Data();
}

/*
*	Returns a new components index for point light entities.
*/
size_t ComponentManager::GetNewPointLightEntitiesEntityComponentsIndex() CATALYST_NOEXCEPT
{
	//Create the relevant components.
	pointLightEntityPointLightComponents.Emplace();

	//Return the new index.
	return numberOfPointLightEntityComponents++;
}

/*
*	Returns the number of point light entity components.
*/
size_t ComponentManager::GetNumberOfPointLightEntityComponents() CATALYST_NOEXCEPT
{
	//Return the number of point light entity components.
	return numberOfPointLightEntityComponents;
}

/*
*	Returns the point light entity point light components.
*/
CATALYST_RESTRICTED PointLightComponent* ComponentManager::GetPointLightEntityPointLightComponents() CATALYST_NOEXCEPT
{
	//Return the point light entity point light components.
	return pointLightEntityPointLightComponents.Data();
}

/*
*	Returns a new components index for spot light entities.
*/
size_t ComponentManager::GetNewSpotLightEntitiesEntityComponentsIndex() CATALYST_NOEXCEPT
{
	//Create the relevant components.
	spotLightEntitySpotLightComponents.Emplace();

	//Return the new index.
	return numberOfSpotLightEntityComponents++;
}

/*
*	Returns the number of spot light entity components.
*/
size_t ComponentManager::GetNumberOfSpotLightEntityComponents() CATALYST_NOEXCEPT
{
	//Return the number of spot light entity components.
	return numberOfSpotLightEntityComponents;
}

/*
*	Returns the spot light entity spot light components.
*/
CATALYST_RESTRICTED SpotLightComponent* ComponentManager::GetSpotLightEntitySpotLightComponents() CATALYST_NOEXCEPT
{
	//Return the spot light entity spot light components.
	return spotLightEntitySpotLightComponents.Data();
}