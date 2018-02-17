//Header file.
#include <ComponentManager.h>

//Static variable definitions.
size_t ComponentManager::numberOfCameraComponents = 0;
DynamicArray<CameraComponent> ComponentManager::cameraComponents;

size_t ComponentManager::numberOfDirectionalLightComponents = 0;
DynamicArray<DirectionalLightComponent> ComponentManager::directionalLightComponents;

size_t ComponentManager::numberOfPhysicalComponents = 0;
DynamicArray<FrustumCullingComponent> ComponentManager::physicalFrustumCullingComponents;
DynamicArray<GraphicsBufferComponent> ComponentManager::physicalGraphicsBufferComponents;
DynamicArray<RenderComponent> ComponentManager::physicalRenderComponents;
DynamicArray<TransformComponent> ComponentManager::physicalTransformComponents;

size_t ComponentManager::numberOfPointLightComponents = 0;
DynamicArray<PointLightComponent> ComponentManager::pointLightComponents;

size_t ComponentManager::numberOfSpotLightComponents = 0;
DynamicArray<SpotLightComponent> ComponentManager::spotLightComponents;

size_t ComponentManager::numberOfTerrainComponents = 0;
DynamicArray<TerrainComponent> ComponentManager::terrainComponents;
DynamicArray<TerrainRenderComponent> ComponentManager::terrainRenderComponents;
;

/*
*	Returns a new components index for camera entities.
*/
size_t ComponentManager::GetNewCameraComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	cameraComponents.EmplaceSlow();

	//Return the new index.
	return numberOfCameraComponents++;
}

/*
*	Returns the number of camera components.
*/
size_t ComponentManager::GetNumberOfCameraComponents() NOEXCEPT
{
	//Return the number of camera entity components.
	return numberOfCameraComponents;
}

/*
*	Returns the camera components.
*/
RESTRICTED CameraComponent* ComponentManager::GetCameraComponents() NOEXCEPT
{
	//Return the camera components.
	return cameraComponents.Data();
}

/*
*	Returns a new components index for directional light entities.
*/
size_t ComponentManager::GetNewDirectionalLightComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	directionalLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfDirectionalLightComponents++;
}

/*
*	Returns the number of directional light components.
*/
size_t ComponentManager::GetNumberOfDirectionalLightComponents() NOEXCEPT
{
	//Return the number of directional light components.
	return numberOfDirectionalLightComponents;
}

/*
*	Returns the directional light components.
*/
RESTRICTED DirectionalLightComponent* ComponentManager::GetDirectionalLightComponents() NOEXCEPT
{
	//Return the directional light components.
	return directionalLightComponents.Data();
}

/*
*	Returns a new components index for physical entities.
*/
size_t ComponentManager::GetNewPhysicalComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	physicalFrustumCullingComponents.EmplaceSlow();
	physicalGraphicsBufferComponents.EmplaceSlow();
	physicalRenderComponents.EmplaceSlow();
	physicalTransformComponents.EmplaceSlow();

	//Return the new index.
	return numberOfPhysicalComponents++;
}

/*
*	Returns the number of physical components.
*/
size_t ComponentManager::GetNumberOfPhysicalComponents() NOEXCEPT
{
	//Return the number of physical components.
	return numberOfPhysicalComponents;
}

/*
*	Returns the physical frustum culling components.
*/
RESTRICTED FrustumCullingComponent* ComponentManager::GetPhysicalFrustumCullingComponents() NOEXCEPT
{
	//Return the physical frustum culling components.
	return physicalFrustumCullingComponents.Data();
}

/*
*	Returns the physical graphics buffer components.
*/
RESTRICTED GraphicsBufferComponent* ComponentManager::GetPhysicalGraphicsBufferComponents() NOEXCEPT
{
	//Return the physical graphics buffer components.
	return physicalGraphicsBufferComponents.Data();
}

/*
*	Returns the physical render components.
*/
RESTRICTED RenderComponent* ComponentManager::GetPhysicalRenderComponents() NOEXCEPT
{
	//Return the physical render components.
	return physicalRenderComponents.Data();
}

/*
*	Returns the physical frustum transform components.
*/
RESTRICTED TransformComponent* ComponentManager::GetPhysicalTransformComponents() NOEXCEPT
{
	//Return the physical frustum transform components.
	return physicalTransformComponents.Data();
}

/*
*	Returns a new components index for point light entities.
*/
size_t ComponentManager::GetNewPointLightComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	pointLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfPointLightComponents++;
}

/*
*	Returns the number of point light components.
*/
size_t ComponentManager::GetNumberOfPointLightComponents() NOEXCEPT
{
	//Return the number of point light components.
	return numberOfPointLightComponents;
}

/*
*	Returns the point light components.
*/
RESTRICTED PointLightComponent* ComponentManager::GetPointLightComponents() NOEXCEPT
{
	//Return the point light components.
	return pointLightComponents.Data();
}

/*
*	Returns a new components index for spot light entities.
*/
size_t ComponentManager::GetNewSpotLightComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	spotLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfSpotLightComponents++;
}

/*
*	Returns the number of spot light components.
*/
size_t ComponentManager::GetNumberOfSpotLightComponents() NOEXCEPT
{
	//Return the number of spot light components.
	return numberOfSpotLightComponents;
}

/*
*	Returns the spot light components.
*/
RESTRICTED SpotLightComponent* ComponentManager::GetSpotLightComponents() NOEXCEPT
{
	//Return the spot light components.
	return spotLightComponents.Data();
}

/*
*	Returns a new components index for terrain entities.
*/
size_t ComponentManager::GetNewTerrainComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	terrainComponents.EmplaceSlow();
	terrainRenderComponents.EmplaceSlow();

	//Return the new index.
	return numberOfTerrainComponents++;
}

/*
*	Returns the number of terrain components.
*/
size_t ComponentManager::GetNumberOfTerrainComponents() NOEXCEPT
{
	//Return the number of terrain components.
	return numberOfTerrainComponents;
}

/*
*	Returns the terrain components.
*/
RESTRICTED TerrainComponent* ComponentManager::GetTerrainComponents() NOEXCEPT
{
	//Return the terrain components.
	return terrainComponents.Data();
}

/*
*	Returns the terrain render components.
*/
RESTRICTED TerrainRenderComponent* ComponentManager::GetTerrainRenderComponents() NOEXCEPT
{
	//Return the terrain render components.
	return terrainRenderComponents.Data();
}