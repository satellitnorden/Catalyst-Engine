#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <CameraComponent.h>
#include <DirectionalLightComponent.h>
#include <FrustumCullingComponent.h>
#include <GraphicsBufferComponent.h>
#include <RenderComponent.h>
#include <PointLightComponent.h>
#include <SpotLightComponent.h>
#include <TerrainComponent.h>
#include <TransformComponent.h>

class ComponentManager final
{

public:

	/*								*/
	/*	Camera Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for camera entities.
	*/
	static size_t GetNewCameraEntityComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of camera entity components.
	*/
	static size_t GetNumberOfCameraEntityComponents() NOEXCEPT;

	/*
	*	Returns the camera entity camera components.
	*/
	RESTRICTED static CameraComponent* GetCameraEntityCameraComponents() NOEXCEPT;

private:

	//The number of camera entity components.
	static size_t numberOfCameraEntityComponents;

	//The camera entity camera components.
	static DynamicArray<CameraComponent> cameraEntityCameraComponents;

public:

	/*										*/
	/*	Directional Light Entity Interface.	*/
	/*										*/

	/*
	*	Returns a new components index for directional light entities.
	*/
	static size_t GetNewDirectionalLightEntityComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of directional light entity components.
	*/
	static size_t GetNumberOfDirectionalLightEntityComponents() NOEXCEPT;

	/*
	*	Returns the directional light entity directional light components.
	*/
	RESTRICTED static DirectionalLightComponent* GetDirectionalLightEntityDirectionalLightComponents() NOEXCEPT;

private:

	//The number of directional lighgt entity components.
	static size_t numberOfDirectionalLightEntityComponents;

	//The the directional light entity directional light components.
	static DynamicArray<DirectionalLightComponent> directionalLightEntityDirectionalLightComponents;

public:

	/*								*/
	/*	Physical Entity Interface.	*/
	/*								*/						

	/*
	*	Returns a new components index for physical entities.
	*/
	static size_t GetNewPhysicalEntityComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of physical entity components.
	*/
	static size_t GetNumberOfPhysicalEntityComponents() NOEXCEPT;

	/*
	*	Returns the physical entity frustum culling components.
	*/
	RESTRICTED static FrustumCullingComponent* GetPhysicalEntityFrustumCullingComponents() NOEXCEPT;

	/*
	*	Returns the physical entity graphics buffer components.
	*/
	RESTRICTED static GraphicsBufferComponent* GetPhysicalEntityGraphicsBufferComponents() NOEXCEPT;

	/*
	*	Returns the physical entity render components.
	*/
	RESTRICTED static RenderComponent* GetPhysicalEntityRenderComponents() NOEXCEPT;

	/*
	*	Returns the physical entity transform components.
	*/
	RESTRICTED static TransformComponent* GetPhysicalEntityTransformComponents() NOEXCEPT;

private:

	//The number of physical entity components.
	static size_t numberOfPhysicalEntityComponents;

	//The physical entity frustum culling components.
	static DynamicArray<FrustumCullingComponent> physicalEntityFrustumCullingComponents;

	//The physical entity graphics buffer components.
	static DynamicArray<GraphicsBufferComponent> physicalEntityGraphicsBufferComponents;

	//The physical entity render components.
	static DynamicArray<RenderComponent> physicalEntityRenderComponents;

	//The physical entity transform components.
	static DynamicArray<TransformComponent> physicalEntityTransformComponents;

public:

	/*									*/
	/*	Point Light Entity Interface.	*/
	/*									*/

	/*
	*	Returns a new components index for point light entities.
	*/
	static size_t GetNewPointLightEntitiesEntityComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of point light entity components.
	*/
	static size_t GetNumberOfPointLightEntityComponents() NOEXCEPT;

	/*
	*	Returns the point light entity point light components.
	*/
	RESTRICTED static PointLightComponent* GetPointLightEntityPointLightComponents() NOEXCEPT;

private:

	//The number of point light entity components.
	static size_t numberOfPointLightEntityComponents;

	//The point light entity point light components.
	static DynamicArray<PointLightComponent> pointLightEntityPointLightComponents;

public:

	/*									*/
	/*	Spot Light Entity Interface.	*/
	/*									*/

	/*
	*	Returns a new components index for spot light entities.
	*/
	static size_t GetNewSpotLightEntitiesEntityComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of spot light entity components.
	*/
	static size_t GetNumberOfSpotLightEntityComponents() NOEXCEPT;

	/*
	*	Returns the spot light entity spot light components.
	*/
	RESTRICTED static SpotLightComponent* GetSpotLightEntitySpotLightComponents() NOEXCEPT;

private:

	//The number of spot light entity components.
	static size_t numberOfSpotLightEntityComponents;

	//The spot light entity spot light components.
	static DynamicArray<SpotLightComponent> spotLightEntitySpotLightComponents;

public:

	/*								*/
	/*	Terrain Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for terrain entities.
	*/
	static size_t GetNewTerrainEntityComponentsIndex() NOEXCEPT;

	/*
	*	Returns the number of terrain entity components.
	*/
	static size_t GetNumberOfTerrainEntityComponents() NOEXCEPT;

	/*
	*	Returns the terrain entity terrain components.
	*/
	RESTRICTED static TerrainComponent* GetTerrainEntityTerrainComponents() NOEXCEPT;

private:

	//The number of terrain entity components.
	static size_t numberOfTerrainEntityComponents;

	//The terrain entity terrain components.
	static DynamicArray<TerrainComponent> terrainEntityTerrainComponents;

};