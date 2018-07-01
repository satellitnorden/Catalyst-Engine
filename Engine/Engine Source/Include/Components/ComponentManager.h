#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Components.
#include <Components/CameraComponent.h>
#include <Components/DirectionalLightComponent.h>
#include <Components/FrustumCullingComponent.h>
#include <Components/InstancedPhysicalRenderComponent.h>
#include <Components/ParticleSystemComponent.h>
#include <Components/ParticleSystemRenderComponent.h>
#include <Components/PointLightComponent.h>
#include <Components/Sound2DComponent.h>
#include <Components/Sound3DComponent.h>
#include <Components/SpotLightComponent.h>
#include <Components/StaticPhysicalRenderComponent.h>
#include <Components/TerrainComponent.h>
#include <Components/TerrainRenderComponent.h>
#include <Components/TransformComponent.h>
#include <Components/VegetationComponent.h>
#include <Components/VegetationCullingComponent.h>
#include <Components/WaterComponent.h>
#include <Components/WaterRenderComponent.h>

//Forward declarations.
class Entity;

class ComponentManager final
{

public:

	/*								*/
	/*	Camera Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for camera entities.
	*/
	static uint64 GetNewCameraComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of camera components.
	*/
	static uint64 GetNumberOfCameraComponents() NOEXCEPT;

	/*
	*	Returns the camera components.
	*/
	static CameraComponent *RESTRICT GetCameraComponents() NOEXCEPT;

private:

	//The number of camera components.
	static std::atomic<uint64> numberOfCameraComponents;

	//The camera components.
	static DynamicArray<CameraComponent> cameraComponents;

public:

	/*										*/
	/*	Directional Light Entity Interface.	*/
	/*										*/

	/*
	*	Returns a new components index for directional light entities.
	*/
	static uint64 GetNewDirectionalLightComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of directional light components.
	*/
	static uint64 GetNumberOfDirectionalLightComponents() NOEXCEPT;

	/*
	*	Returns the directional light components.
	*/
	static DirectionalLightComponent *RESTRICT GetDirectionalLightComponents() NOEXCEPT;

private:

	//The number of directional light components.
	static std::atomic<uint64> numberOfDirectionalLightComponents;

	//The the directional light components.
	static DynamicArray<DirectionalLightComponent> directionalLightComponents;

public:

	/*											*/
	/*	Instanced Physical Entity Interface.	*/
	/*											*/

	/*
	*	Returns a new components index for instanced physical entities.
	*/
	static uint64 GetNewInstancedPhysicalComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of instanced physical components.
	*/
	static uint64 GetNumberOfInstancedPhysicalComponents() NOEXCEPT;

	/*
	*	Returns the instanced physical render components.
	*/
	static InstancedPhysicalRenderComponent *RESTRICT GetInstancedPhysicalRenderComponents() NOEXCEPT;

private:

	//The number of instanced physical components.
	static std::atomic<uint64> numberOfInstancedPhysicalComponents;

	//The instanced physical render components.
	static DynamicArray<InstancedPhysicalRenderComponent> instancedPhysicalRenderComponents;

public:

	/*										*/
	/*	Static Physical Entity Interface.	*/
	/*										*/						

	/*
	*	Returns a new components index for static physical entities.
	*/
	static uint64 GetNewStaticPhysicalComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of static physical components.
	*/
	static uint64 GetNumberOfStaticPhysicalComponents() NOEXCEPT;

	/*
	*	Returns the static physical frustum culling components.
	*/
	static FrustumCullingComponent *RESTRICT GetStaticPhysicalFrustumCullingComponents() NOEXCEPT;

	/*
	*	Returns the static physical render components.
	*/
	static StaticPhysicalRenderComponent *RESTRICT GetStaticPhysicalRenderComponents() NOEXCEPT;

	/*
	*	Returns the static physical transform components.
	*/
	static TransformComponent *RESTRICT GetStaticPhysicalTransformComponents() NOEXCEPT;

private:

	//The number of static physical components.
	static std::atomic<uint64> numberOfStaticPhysicalComponents;

	//The static physical frustum culling components.
	static DynamicArray<FrustumCullingComponent> staticPhysicalFrustumCullingComponents;

	//The static physical render components.
	static DynamicArray<StaticPhysicalRenderComponent> staticPhysicalRenderComponents;

	//The static physical transform components.
	static DynamicArray<TransformComponent> staticPhysicalTransformComponents;

public:

	/*										*/
	/*	Particle System Entity Interface.	*/
	/*										*/

	/*
	*	Returns a new components index for particle system entities.
	*/
	static uint64 GetNewParticleSystemComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of particle system components.
	*/
	static uint64 GetNumberOfParticleSystemComponents() NOEXCEPT;

	/*
	*	Returns the particle system components.
	*/
	static ParticleSystemComponent *RESTRICT GetParticleSystemComponents() NOEXCEPT;

	/*
	*	Returns the particle system render components.
	*/
	static ParticleSystemRenderComponent *RESTRICT GetParticleSystemRenderComponents() NOEXCEPT;

private:

	//The number of particle system components.
	static std::atomic<uint64> numberOfParticleSystemComponents;

	//The particle system components.
	static DynamicArray<ParticleSystemComponent> particleSystemComponents;
	
	//The particle system render components.
	static DynamicArray<ParticleSystemRenderComponent> particleSystemRenderComponents;

public:

	/*									*/
	/*	Point Light Entity Interface.	*/
	/*									*/

	/*
	*	Returns a new components index for point light entities.
	*/
	static uint64 GetNewPointLightComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of point light components.
	*/
	static uint64 GetNumberOfPointLightComponents() NOEXCEPT;

	/*
	*	Returns the point light components.
	*/
	static PointLightComponent *RESTRICT GetPointLightComponents() NOEXCEPT;

private:

	//The number of point light components.
	static std::atomic<uint64> numberOfPointLightComponents;

	//The point light components.
	static DynamicArray<PointLightComponent> pointLightComponents;

public:

	/*								*/
	/*	Sound 2D Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for sound 2D entities.
	*/
	static uint64 GetNewSound2DComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of sound 2D components.
	*/
	static uint64 GetNumberOfSound2DComponents() NOEXCEPT;

	/*
	*	Returns the sound 2D components.
	*/
	static Sound2DComponent *RESTRICT GetSound2DComponents() NOEXCEPT;

private:

	//The number of sound 2D components.
	static std::atomic<uint64> numberOfSound2DComponents;

	//The sound 2D components.
	static DynamicArray<Sound2DComponent> sound2DComponents;

public:

	/*								*/
	/*	Sound 3D Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for sound 3D entities.
	*/
	static uint64 GetNewSound3DComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of sound 3D components.
	*/
	static uint64 GetNumberOfSound3DComponents() NOEXCEPT;

	/*
	*	Returns the sound 3D components.
	*/
	static Sound3DComponent *RESTRICT GetSound3DComponents() NOEXCEPT;

private:

	//The number of sound 3D components.
	static std::atomic<uint64> numberOfSound3DComponents;

	//The sound 3D components.
	static DynamicArray<Sound3DComponent> sound3DComponents;

public:

	/*									*/
	/*	Spot Light Entity Interface.	*/
	/*									*/

	/*
	*	Returns a new components index for spot light entities.
	*/
	static uint64 GetNewSpotLightComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of spot light components.
	*/
	static uint64 GetNumberOfSpotLightComponents() NOEXCEPT;

	/*
	*	Returns the spot light components.
	*/
	static SpotLightComponent *RESTRICT GetSpotLightComponents() NOEXCEPT;

private:

	//The number of spot light components.
	static std::atomic<uint64> numberOfSpotLightComponents;

	//The spot light components.
	static DynamicArray<SpotLightComponent> spotLightComponents;

public:

	/*								*/
	/*	Terrain Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for terrain entities.
	*/
	static uint64 GetNewTerrainComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Returns the number of terrain components.
	*/
	static uint64 GetNumberOfTerrainComponents() NOEXCEPT;

	/*
	*	Returns the terrain components.
	*/
	static TerrainComponent *RESTRICT GetTerrainComponents() NOEXCEPT;

	/*
	*	Returns the terrain frustum culling components.
	*/
	static FrustumCullingComponent *const RESTRICT GetTerrainFrustumCullingComponents() NOEXCEPT;

	/*
	*	Returns the terrain render components.
	*/
	static TerrainRenderComponent *RESTRICT GetTerrainRenderComponents() NOEXCEPT;

	/*
	*	Returns a components index for terrain entities.
	*/
	static void ReturnTerrainComponentsIndex(const uint64 componentsIndex) NOEXCEPT;

private:

	//The terrain entities.
	static DynamicArray<Entity *RESTRICT> terrainEntities;

	//The terrain components.
	static DynamicArray<TerrainComponent> terrainComponents;

	//The terrain frustum culling components.
	static DynamicArray<FrustumCullingComponent> terrainFrustumCullingComponents;

	//The terrain render components.
	static DynamicArray<TerrainRenderComponent> terrainRenderComponents;

public:

	/*									*/
	/*	Vegetation Entity Interface.	*/
	/*									*/

	/*
	*	Returns a new components index for vegetation entities.
	*/
	static uint64 GetNewVegetationComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of vegetation components.
	*/
	static uint64 GetNumberOfVegetationComponents() NOEXCEPT;

	/*
	*	Returns the vegetation components.
	*/
	static VegetationComponent *RESTRICT GetVegetationComponents() NOEXCEPT;

	/*
	*	Returns the vegetation culling components.
	*/
	static VegetationCullingComponent *RESTRICT GetVegetationCullingComponents() NOEXCEPT;

private:

	//The number of vegetation components.
	static std::atomic<uint64> numberOfVegetationComponents;

	//The vegetation components.
	static DynamicArray<VegetationComponent> vegetationComponents;

	//The vegetation culling components.
	static DynamicArray<VegetationCullingComponent> vegetationCullingComponents;

public:

	/*							*/
	/*	Water Entity Interface.	*/
	/*							*/

	/*
	*	Returns a new components index for water entities.
	*/
	static uint64 GetNewWaterComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of water components.
	*/
	static uint64 GetNumberOfWaterComponents() NOEXCEPT;

	/*
	*	Returns the water components.
	*/
	static WaterComponent *RESTRICT GetWaterComponents() NOEXCEPT;

	/*
	*	Returns the water render components.
	*/
	static WaterRenderComponent *RESTRICT GetWaterRenderComponents() NOEXCEPT;

private:

	//The number of water components.
	static std::atomic<uint64> numberOfWaterComponents;

	//The water components.
	static DynamicArray<WaterComponent> waterComponents;

	//The water render components.
	static DynamicArray<WaterRenderComponent> waterRenderComponents;

};