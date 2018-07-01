//Header file.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>

//Static variable definitions.
std::atomic<uint64> ComponentManager::numberOfCameraComponents = 0;
DynamicArray<CameraComponent> ComponentManager::cameraComponents;

std::atomic<uint64> ComponentManager::numberOfDirectionalLightComponents = 0;
DynamicArray<DirectionalLightComponent> ComponentManager::directionalLightComponents;

std::atomic<uint64> ComponentManager::numberOfInstancedPhysicalComponents = 0;
DynamicArray<InstancedPhysicalRenderComponent> ComponentManager::instancedPhysicalRenderComponents;

std::atomic<uint64> ComponentManager::numberOfStaticPhysicalComponents = 0;
DynamicArray<FrustumCullingComponent> ComponentManager::staticPhysicalFrustumCullingComponents;
DynamicArray<StaticPhysicalRenderComponent> ComponentManager::staticPhysicalRenderComponents;
DynamicArray<TransformComponent> ComponentManager::staticPhysicalTransformComponents;

std::atomic<uint64> ComponentManager::numberOfParticleSystemComponents = 0;
DynamicArray<ParticleSystemComponent> ComponentManager::particleSystemComponents;
DynamicArray<ParticleSystemRenderComponent> ComponentManager::particleSystemRenderComponents;

std::atomic<uint64> ComponentManager::numberOfPointLightComponents = 0;
DynamicArray<PointLightComponent> ComponentManager::pointLightComponents;

std::atomic<uint64> ComponentManager::numberOfSound2DComponents = 0;
DynamicArray<Sound2DComponent> ComponentManager::sound2DComponents;

std::atomic<uint64> ComponentManager::numberOfSound3DComponents = 0;
DynamicArray<Sound3DComponent> ComponentManager::sound3DComponents;

std::atomic<uint64> ComponentManager::numberOfSpotLightComponents = 0;
DynamicArray<SpotLightComponent> ComponentManager::spotLightComponents;

DynamicArray<Entity *RESTRICT> ComponentManager::terrainEntities;
DynamicArray<TerrainComponent> ComponentManager::terrainComponents;
DynamicArray<FrustumCullingComponent> ComponentManager::terrainFrustumCullingComponents;
DynamicArray<TerrainRenderComponent> ComponentManager::terrainRenderComponents;

std::atomic<uint64> ComponentManager::numberOfVegetationComponents = 0;
DynamicArray<VegetationComponent> ComponentManager::vegetationComponents;
DynamicArray<VegetationCullingComponent> ComponentManager::vegetationCullingComponents;

std::atomic<uint64> ComponentManager::numberOfWaterComponents = 0;
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
*	Returns a new components index for particle system entities.
*/
uint64 ComponentManager::GetNewParticleSystemComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	particleSystemComponents.EmplaceSlow();
	particleSystemRenderComponents.EmplaceSlow();

	//Return the new index.
	return numberOfParticleSystemComponents++;
}

/*
*	Returns the number of particle system components.
*/
uint64 ComponentManager::GetNumberOfParticleSystemComponents() NOEXCEPT
{
	//Return the number of particle system components.
	return numberOfParticleSystemComponents;
}

/*
*	Returns the particle system components.
*/
ParticleSystemComponent *RESTRICT ComponentManager::GetParticleSystemComponents() NOEXCEPT
{
	//Return the particle system components.
	return particleSystemComponents.Data();
}

/*
*	Returns the particle system render components.
*/
ParticleSystemRenderComponent *RESTRICT ComponentManager::GetParticleSystemRenderComponents() NOEXCEPT
{
	//Return the particle system render components.
	return particleSystemRenderComponents.Data();
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
*	Returns a new components index for sound 2D entities.
*/
uint64 ComponentManager::GetNewSound2DComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	sound2DComponents.EmplaceSlow();

	//Return the new index.
	return numberOfSound2DComponents++;
}

/*
*	Returns the number of sound 2D components.
*/
uint64 ComponentManager::GetNumberOfSound2DComponents() NOEXCEPT
{
	//Return the number of sound 2D components.
	return numberOfSound2DComponents;
}

/*
*	Returns the sound 2D components.
*/
Sound2DComponent *RESTRICT ComponentManager::GetSound2DComponents() NOEXCEPT
{
	//Return the sound 2D components.
	return sound2DComponents.Data();
}

/*
*	Returns a new components index for sound 3D entities.
*/
uint64 ComponentManager::GetNewSound3DComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	sound3DComponents.EmplaceSlow();

	//Return the new index.
	return numberOfSound3DComponents++;
}

/*
*	Returns the number of sound 3D components.
*/
uint64 ComponentManager::GetNumberOfSound3DComponents() NOEXCEPT
{
	//Return the number of sound 3D components.
	return numberOfSound3DComponents;
}

/*
*	Returns the sound 3D components.
*/
Sound3DComponent *RESTRICT ComponentManager::GetSound3DComponents() NOEXCEPT
{
	//Return the sound 3D components.
	return sound3DComponents.Data();
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
uint64 ComponentManager::GetNewTerrainComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT
{
	//Create the relevant components.
	terrainEntities.EmplaceSlow(entity);
	terrainComponents.EmplaceSlow();
	terrainFrustumCullingComponents.EmplaceSlow();
	terrainRenderComponents.EmplaceSlow();

	//Return the new index.
	return terrainEntities.LastIndex();
}

/*
*	Returns the number of terrain components.
*/
uint64 ComponentManager::GetNumberOfTerrainComponents() NOEXCEPT
{
	//Return the number of terrain components.
	return terrainEntities.Size();
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
*	Returns the terrain frustum culling components.
*/
FrustumCullingComponent *const RESTRICT ComponentManager::GetTerrainFrustumCullingComponents() NOEXCEPT
{
	//Return the terrain frustum culling components.
	return terrainFrustumCullingComponents.Data();
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
*	Returns a components index for terrain entities.
*/
void ComponentManager::ReturnTerrainComponentsIndex(const uint64 componentsIndex) NOEXCEPT
{
	//Tell the entity at the back that it is getting a new components index.
	terrainEntities.Back()->SetComponentsIndex(componentsIndex);

	//Erase the components.
	terrainEntities.Erase(componentsIndex);
	terrainComponents.Erase(componentsIndex);
	terrainFrustumCullingComponents.Erase(componentsIndex);
	terrainRenderComponents.Erase(componentsIndex);
}

/*
*	Returns a new components index for vegetation entities.
*/
uint64 ComponentManager::GetNewVegetationComponentsIndex() NOEXCEPT
{
	//Create the relevant components.
	vegetationComponents.EmplaceSlow();
	vegetationCullingComponents.EmplaceSlow();

	//Return the new index.
	return numberOfVegetationComponents++;
}

/*
*	Returns the number of vegetation components.
*/
uint64 ComponentManager::GetNumberOfVegetationComponents() NOEXCEPT
{
	//Return the number of vegetation components.
	return numberOfVegetationComponents;
}

/*
*	Returns the vegetation components.
*/
VegetationComponent *RESTRICT ComponentManager::GetVegetationComponents() NOEXCEPT
{
	//Return the vegetation components.
	return vegetationComponents.Data();
}

/*
*	Returns the vegetation culling components.
*/
VegetationCullingComponent *RESTRICT ComponentManager::GetVegetationCullingComponents() NOEXCEPT
{
	//Return the vegetation components.
	return vegetationCullingComponents.Data();
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