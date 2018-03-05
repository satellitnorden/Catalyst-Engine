//Header file.
#include <ComponentManager.h>

//Static variable definitions.
uint64 ComponentManager::numberOfCameraComponents = 0;
DynamicArray<CameraComponent> ComponentManager::cameraComponents;

uint64 ComponentManager::numberOfDirectionalLightComponents = 0;
DynamicArray<DirectionalLightComponent> ComponentManager::directionalLightComponents;

uint64 ComponentManager::numberOfInstancedPhysicalComponents = 0;
DynamicArray<InstancedPhysicalRenderComponent> ComponentManager::instancedPhysicalRenderComponents;

uint64 ComponentManager::numberOfStaticPhysicalComponents = 0;
DynamicArray<FrustumCullingComponent> ComponentManager::staticPhysicalFrustumCullingComponents;
DynamicArray<StaticPhysicalRenderComponent> ComponentManager::staticPhysicalRenderComponents;
DynamicArray<TransformComponent> ComponentManager::staticPhysicalTransformComponents;

uint64 ComponentManager::numberOfPointLightComponents = 0;
DynamicArray<PointLightComponent> ComponentManager::pointLightComponents;

uint64 ComponentManager::numberOfSpotLightComponents = 0;
DynamicArray<SpotLightComponent> ComponentManager::spotLightComponents;

uint64 ComponentManager::numberOfTerrainComponents = 0;
DynamicArray<TerrainComponent> ComponentManager::terrainComponents;
DynamicArray<TerrainRenderComponent> ComponentManager::terrainRenderComponents;

uint64 ComponentManager::numberOfWaterComponents = 0;
DynamicArray<WaterComponent> ComponentManager::waterComponents;
DynamicArray<WaterRenderComponent> ComponentManager::waterRenderComponents;

/*
*	Returns a new components index for camera entities.
*/
uint64 ComponentManager::GetNewCameraComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	cameraComponents.EmplaceSlow();

	//Return the new index.
	return numberOfCameraComponents++;
}

/*
*	Returns the number of camera components.
*/
uint64 ComponentManager::GetNumberOfCameraComponents() NOEXCEPT
{
	//Return the number of camera entity components.
	return numberOfCameraComponents;
}

/*
*	Returns the camera components.
*/
CameraComponent *RESTRICT ComponentManager::GetCameraComponents() NOEXCEPT
{
	//Return the camera components.
	return cameraComponents.Data();
}

/*
*	Returns a new components index for directional light entities.
*/
uint64 ComponentManager::GetNewDirectionalLightComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	directionalLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfDirectionalLightComponents++;
}

/*
*	Returns the number of directional light components.
*/
uint64 ComponentManager::GetNumberOfDirectionalLightComponents() NOEXCEPT
{
	//Return the number of directional light components.
	return numberOfDirectionalLightComponents;
}

/*
*	Returns the directional light components.
*/
DirectionalLightComponent *RESTRICT ComponentManager::GetDirectionalLightComponents() NOEXCEPT
{
	//Return the directional light components.
	return directionalLightComponents.Data();
}

/*
*	Returns a new components index for instanced physical entities.
*/
uint64 ComponentManager::GetNewInstancedPhysicalComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	instancedPhysicalRenderComponents.EmplaceSlow();

	//Return the new index.
	return numberOfInstancedPhysicalComponents++;
}

/*
*	Returns the number of instanced physical components.
*/
uint64 ComponentManager::GetNumberOfInstancedPhysicalComponents() NOEXCEPT
{
	//Return the number of instanced physical components.
	return numberOfInstancedPhysicalComponents;
}

/*
*	Returns the instanced physical render components.
*/
InstancedPhysicalRenderComponent *RESTRICT ComponentManager::GetInstancedPhysicalRenderComponents() NOEXCEPT
{
	//Return the instanced physical render components.
	return instancedPhysicalRenderComponents.Data();
}

/*
*	Returns a new components index for static physical entities.
*/
uint64 ComponentManager::GetNewStaticPhysicalComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	staticPhysicalFrustumCullingComponents.EmplaceSlow();
	staticPhysicalRenderComponents.EmplaceSlow();
	staticPhysicalTransformComponents.EmplaceSlow();

	//Return the new index.
	return numberOfStaticPhysicalComponents++;
}

/*
*	Returns the number of static physical components.
*/
uint64 ComponentManager::GetNumberOfStaticPhysicalComponents() NOEXCEPT
{
	//Return the number of physical components.
	return numberOfStaticPhysicalComponents;
}

/*
*	Returns the static physical frustum culling components.
*/
FrustumCullingComponent *RESTRICT ComponentManager::GetStaticPhysicalFrustumCullingComponents() NOEXCEPT
{
	//Return the physical frustum culling components.
	return staticPhysicalFrustumCullingComponents.Data();
}

/*
*	Returns the static physical render components.
*/
StaticPhysicalRenderComponent *RESTRICT ComponentManager::GetStaticPhysicalRenderComponents() NOEXCEPT
{
	//Return the physical render components.
	return staticPhysicalRenderComponents.Data();
}

/*
*	Returns the static physical frustum transform components.
*/
TransformComponent *RESTRICT ComponentManager::GetStaticPhysicalTransformComponents() NOEXCEPT
{
	//Return the physical frustum transform components.
	return staticPhysicalTransformComponents.Data();
}

/*
*	Returns a new components index for point light entities.
*/
uint64 ComponentManager::GetNewPointLightComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	pointLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfPointLightComponents++;
}

/*
*	Returns the number of point light components.
*/
uint64 ComponentManager::GetNumberOfPointLightComponents() NOEXCEPT
{
	//Return the number of point light components.
	return numberOfPointLightComponents;
}

/*
*	Returns the point light components.
*/
PointLightComponent *RESTRICT ComponentManager::GetPointLightComponents() NOEXCEPT
{
	//Return the point light components.
	return pointLightComponents.Data();
}

/*
*	Returns a new components index for spot light entities.
*/
uint64 ComponentManager::GetNewSpotLightComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	spotLightComponents.EmplaceSlow();

	//Return the new index.
	return numberOfSpotLightComponents++;
}

/*
*	Returns the number of spot light components.
*/
uint64 ComponentManager::GetNumberOfSpotLightComponents() NOEXCEPT
{
	//Return the number of spot light components.
	return numberOfSpotLightComponents;
}

/*
*	Returns the spot light components.
*/
SpotLightComponent *RESTRICT ComponentManager::GetSpotLightComponents() NOEXCEPT
{
	//Return the spot light components.
	return spotLightComponents.Data();
}

/*
*	Returns a new components index for terrain entities.
*/
uint64 ComponentManager::GetNewTerrainComponentsIndex() NOEXCEPT
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
uint64 ComponentManager::GetNumberOfTerrainComponents() NOEXCEPT
{
	//Return the number of terrain components.
	return numberOfTerrainComponents;
}

/*
*	Returns the terrain components.
*/
TerrainComponent *RESTRICT ComponentManager::GetTerrainComponents() NOEXCEPT
{
	//Return the terrain components.
	return terrainComponents.Data();
}

/*
*	Returns the terrain render components.
*/
TerrainRenderComponent *RESTRICT ComponentManager::GetTerrainRenderComponents() NOEXCEPT
{
	//Return the terrain render components.
	return terrainRenderComponents.Data();
}

/*
*	Returns a new components index for water entities.
*/
uint64 ComponentManager::GetNewWaterComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	waterComponents.EmplaceSlow();
	waterRenderComponents.EmplaceSlow();

	//Return the new index.
	return numberOfWaterComponents++;
}

/*
*	Returns the number of water components.
*/
uint64 ComponentManager::GetNumberOfWaterComponents() NOEXCEPT
{
	//Return the number of water components.
	return numberOfWaterComponents;
}

/*
*	Returns the water components.
*/
WaterComponent *RESTRICT ComponentManager::GetWaterComponents() NOEXCEPT
{
	//Return the water components.
	return waterComponents.Data();
}

/*
*	Returns the water render components.
*/
WaterRenderComponent *RESTRICT ComponentManager::GetWaterRenderComponents() NOEXCEPT
{
	//Return the water render components.
	return waterRenderComponents.Data();
}