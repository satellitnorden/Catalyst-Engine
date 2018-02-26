#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <CameraComponent.h>
#include <DirectionalLightComponent.h>
#include <FrustumCullingComponent.h>
#include <PointLightComponent.h>
#include <SpotLightComponent.h>
#include <StaticPhysicalRenderComponent.h>
#include <TerrainComponent.h>
#include <TerrainRenderComponent.h>
#include <TransformComponent.h>
#include <WaterComponent.h>
#include <WaterRenderComponent.h>

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
	RESTRICTED static CameraComponent* GetCameraComponents() NOEXCEPT;

private:

	//The number of camera components.
	static uint64 numberOfCameraComponents;

	//The camera ecomponents.
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
	RESTRICTED static DirectionalLightComponent* GetDirectionalLightComponents() NOEXCEPT;

private:

	//The number of directional light components.
	static uint64 numberOfDirectionalLightComponents;

	//The the directional light components.
	static DynamicArray<DirectionalLightComponent> directionalLightComponents;

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
	RESTRICTED static FrustumCullingComponent* GetStaticPhysicalFrustumCullingComponents() NOEXCEPT;

	/*
	*	Returns the static physical render components.
	*/
	RESTRICTED static StaticPhysicalRenderComponent* GetStaticPhysicalRenderComponents() NOEXCEPT;

	/*
	*	Returns the static physical transform components.
	*/
	RESTRICTED static TransformComponent* GetStaticPhysicalTransformComponents() NOEXCEPT;

private:

	//The number of static physical components.
	static uint64 numberOfStaticPhysicalComponents;

	//The static physical frustum culling components.
	static DynamicArray<FrustumCullingComponent> staticPhysicalFrustumCullingComponents;

	//The static physical render components.
	static DynamicArray<StaticPhysicalRenderComponent> staticPhysicalRenderComponents;

	//The static physical transform components.
	static DynamicArray<TransformComponent> staticPhysicalTransformComponents;

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
	RESTRICTED static PointLightComponent* GetPointLightComponents() NOEXCEPT;

private:

	//The number of point light components.
	static uint64 numberOfPointLightComponents;

	//The point light components.
	static DynamicArray<PointLightComponent> pointLightComponents;

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
	RESTRICTED static SpotLightComponent* GetSpotLightComponents() NOEXCEPT;

private:

	//The number of spot light components.
	static uint64 numberOfSpotLightComponents;

	//The spot light components.
	static DynamicArray<SpotLightComponent> spotLightComponents;

public:

	/*								*/
	/*	Terrain Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for terrain entities.
	*/
	static uint64 GetNewTerrainComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of terrain components.
	*/
	static uint64 GetNumberOfTerrainComponents() NOEXCEPT;

	/*
	*	Returns the terrain components.
	*/
	RESTRICTED static TerrainComponent* GetTerrainComponents() NOEXCEPT;

	/*
	*	Returns the terrain render components.
	*/
	RESTRICTED static TerrainRenderComponent* GetTerrainRenderComponents() NOEXCEPT;

private:

	//The number of terrain components.
	static uint64 numberOfTerrainComponents;

	//The terrain components.
	static DynamicArray<TerrainComponent> terrainComponents;

	//The terrain render components.
	static DynamicArray<TerrainRenderComponent> terrainRenderComponents;

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
	RESTRICTED static WaterComponent* GetWaterComponents() NOEXCEPT;

	/*
	*	Returns the water render components.
	*/
	RESTRICTED static WaterRenderComponent* GetWaterRenderComponents() NOEXCEPT;

private:

	//The number of water components.
	static uint64 numberOfWaterComponents;

	//The water components.
	static DynamicArray<WaterComponent> waterComponents;

	//The water render components.
	static DynamicArray<WaterRenderComponent> waterRenderComponents;

};