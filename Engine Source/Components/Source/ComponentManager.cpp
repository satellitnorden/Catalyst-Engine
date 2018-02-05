//Header file.
#include <ComponentManager.h>

//Static variable definitions.
size_t ComponentManager::numberOfCameraEntityComponents = 0;
DynamicArray<CameraComponent> ComponentManager::cameraEntityCameraComponents;

size_t ComponentManager::numberOfDirectionalLightEntityComponents = 0;
DynamicArray<DirectionalLightComponent> ComponentManager::directionalLightEntityDirectionalLightComponents;

size_t ComponentManager::numberOfPhysicalEntityComponents = 0;
DynamicArray<FrustumCullingComponent> ComponentManager::physicalEntityFrustumCullingComponents;
DynamicArray<GraphicsBufferComponent> ComponentManager::physicalEntityGraphicsBufferComponents;
DynamicArray<RenderComponent> ComponentManager::physicalEntityRenderComponents;
DynamicArray<TransformComponent> ComponentManager::physicalEntityTransformComponents;

size_t ComponentManager::numberOfPointLightEntityComponents = 0;
DynamicArray<PointLightComponent> ComponentManager::pointLightEntityPointLightComponents;

size_t ComponentManager::numberOfSpotLightEntityComponents = 0;
DynamicArray<SpotLightComponent> ComponentManager::spotLightEntitySpotLightComponents;

size_t ComponentManager::numberOfTerrainEntityComponents = 0;
DynamicArray<TerrainComponent> ComponentManager::terrainEntityTerrainComponents;
DynamicArray<TerrainRenderComponent> ComponentManager::terrainEntityTerrainRenderComponents;
;

/*
*	Returns a new components index for camera entities.
*/
size_t ComponentManager::GetNewCameraEntityComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	cameraEntityCameraComponents.EmplaceSlow();

	//Return the new index.
	return numberOfCameraEntityComponents++;
}

/*
*	Returns the number of camera entity components.
*/
size_t ComponentManager::GetNumberOfCameraEntityComponents() NOEXCEPT
{
	//Return the number of camera entity components.
	return numberOfCameraEntityComponents;
}

/*
*	Returns the camera entity camera components.
*/
RESTRICTED CameraComponent* ComponentManager::GetCameraEntityCameraComponents() NOEXCEPT
{
	//Return the camera entity camera components.
	return cameraEntityCameraComponents.Data();
}

/*
*	Returns a new components index for directional light entities.
*/
size_t ComponentManager::GetNewDirectionalLightEntityComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	directionalLightEntityDirectionalLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfDirectionalLightEntityComponents++;
}

/*
*	Returns the number of directional light entity components.
*/
size_t ComponentManager::GetNumberOfDirectionalLightEntityComponents() NOEXCEPT
{
	//Return the number of directional light entity components.
	return numberOfDirectionalLightEntityComponents;
}

/*
*	Returns the directional light entity directional light components.
*/
RESTRICTED DirectionalLightComponent* ComponentManager::GetDirectionalLightEntityDirectionalLightComponents() NOEXCEPT
{
	//Return the directional light entity directional light components.
	return directionalLightEntityDirectionalLightComponents.Data();
}

/*
*	Returns a new components index for physical entities.
*/
size_t ComponentManager::GetNewPhysicalEntityComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	physicalEntityFrustumCullingComponents.EmplaceSlow();
	physicalEntityGraphicsBufferComponents.EmplaceSlow();
	physicalEntityRenderComponents.EmplaceSlow();
	physicalEntityTransformComponents.EmplaceSlow();

	//Return the new index.
	return numberOfPhysicalEntityComponents++;
}

/*
*	Returns the number of physical entity components.
*/
size_t ComponentManager::GetNumberOfPhysicalEntityComponents() NOEXCEPT
{
	//Return the number of physical entity components.
	return numberOfPhysicalEntityComponents;
}

/*
*	Returns the physical entity frustum culling components.
*/
RESTRICTED FrustumCullingComponent* ComponentManager::GetPhysicalEntityFrustumCullingComponents() NOEXCEPT
{
	//Return the physical entity frustum culling components.
	return physicalEntityFrustumCullingComponents.Data();
}

/*
*	Returns the physical entity graphics buffer components.
*/
RESTRICTED GraphicsBufferComponent* ComponentManager::GetPhysicalEntityGraphicsBufferComponents() NOEXCEPT
{
	//Return the physical entity graphics buffer components.
	return physicalEntityGraphicsBufferComponents.Data();
}

/*
*	Returns the physical entity render components.
*/
RESTRICTED RenderComponent* ComponentManager::GetPhysicalEntityRenderComponents() NOEXCEPT
{
	//Return the physical entity render components.
	return physicalEntityRenderComponents.Data();
}

/*
*	Returns the physical entity frustum transform components.
*/
RESTRICTED TransformComponent* ComponentManager::GetPhysicalEntityTransformComponents() NOEXCEPT
{
	//Return the physical entity frustum transform components.
	return physicalEntityTransformComponents.Data();
}

/*
*	Returns a new components index for point light entities.
*/
size_t ComponentManager::GetNewPointLightEntitiesEntityComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	pointLightEntityPointLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfPointLightEntityComponents++;
}

/*
*	Returns the number of point light entity components.
*/
size_t ComponentManager::GetNumberOfPointLightEntityComponents() NOEXCEPT
{
	//Return the number of point light entity components.
	return numberOfPointLightEntityComponents;
}

/*
*	Returns the point light entity point light components.
*/
RESTRICTED PointLightComponent* ComponentManager::GetPointLightEntityPointLightComponents() NOEXCEPT
{
	//Return the point light entity point light components.
	return pointLightEntityPointLightComponents.Data();
}

/*
*	Returns a new components index for spot light entities.
*/
size_t ComponentManager::GetNewSpotLightEntitiesEntityComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	spotLightEntitySpotLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfSpotLightEntityComponents++;
}

/*
*	Returns the number of spot light entity components.
*/
size_t ComponentManager::GetNumberOfSpotLightEntityComponents() NOEXCEPT
{
	//Return the number of spot light entity components.
	return numberOfSpotLightEntityComponents;
}

/*
*	Returns the spot light entity spot light components.
*/
RESTRICTED SpotLightComponent* ComponentManager::GetSpotLightEntitySpotLightComponents() NOEXCEPT
{
	//Return the spot light entity spot light components.
	return spotLightEntitySpotLightComponents.Data();
}

/*
*	Returns a new components index for terrain entities.
*/
size_t ComponentManager::GetNewTerrainEntityComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	terrainEntityTerrainComponents.EmplaceSlow();
	terrainEntityTerrainRenderComponents.EmplaceSlow();

	//Return the new index.
	return numberOfTerrainEntityComponents++;
}

/*
*	Returns the number of terrain entity components.
*/
size_t ComponentManager::GetNumberOfTerrainEntityComponents() NOEXCEPT
{
	//Return the number of terrain entity components.
	return numberOfTerrainEntityComponents;
}

/*
*	Returns the terrain entity terrain components.
*/
RESTRICTED TerrainComponent* ComponentManager::GetTerrainEntityTerrainComponents() NOEXCEPT
{
	//Return the terrain entity terrain components.
	return terrainEntityTerrainComponents.Data();
}

/*
*	Returns the terrain entity terrain render components.
*/
RESTRICTED TerrainRenderComponent* ComponentManager::GetTerrainEntityTerrainRenderComponents() NOEXCEPT
{
	//Return the terrain entity terrain render components.
	return terrainEntityTerrainRenderComponents.Data();
}