#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <CameraComponent.h>
#include <FrustumCullingComponent.h>
#include <RenderComponent.h>
#include <PointLightComponent.h>
#include <SpotLightComponent.h>
#include <TransformComponent.h>

class ComponentManager
{

public:

	/*								*/
	/*	Camera Entity Interface.	*/
	/*								*/

	/*
	*	Returns a new components index for camera entities.
	*/
	static size_t GetNewCameraEntityComponentsIndex() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of camera entity components.
	*/
	static size_t GetNumberOfCameraEntityComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the camera entity camera components.
	*/
	CATALYST_RESTRICTED static CameraComponent* GetCameraEntityCameraComponents() CATALYST_NOEXCEPT;

private:

	//The number of camera entity components.
	static size_t numberOfCameraEntityComponents;

	//The camera entity camera components.
	static DynamicArray<CameraComponent> cameraEntityCameraComponents;

public:

	/*								*/
	/*	Physical Entity Interface.	*/
	/*								*/						

	/*
	*	Returns a new components index for physical entities.
	*/
	static size_t GetNewPhysicalEntityComponentsIndex() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of physical entity components.
	*/
	static size_t GetNumberOfPhysicalEntityComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the physical entity frustum culling components.
	*/
	CATALYST_RESTRICTED static FrustumCullingComponent* GetPhysicalEntityFrustumCullingComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the physical entity render components.
	*/
	CATALYST_RESTRICTED static RenderComponent* GetPhysicalEntityRenderComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the physical entity transform components.
	*/
	CATALYST_RESTRICTED static TransformComponent* GetPhysicalEntityTransformComponents() CATALYST_NOEXCEPT;

private:

	//The number of physical entity components.
	static size_t numberOfPhysicalEntityComponents;

	//The physical entity frustum culling components.
	static DynamicArray<FrustumCullingComponent> physicalEntityFrustumCullingComponents;

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
	static size_t GetNewPointLightEntitiesEntityComponentsIndex() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of point light entity components.
	*/
	static size_t GetNumberOfPointLightEntityComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the point light entity point light components.
	*/
	CATALYST_RESTRICTED static PointLightComponent* GetPointLightEntityPointLightComponents() CATALYST_NOEXCEPT;

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
	static size_t GetNewSpotLightEntitiesEntityComponentsIndex() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of spot light entity components.
	*/
	static size_t GetNumberOfSpotLightEntityComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the spot light entity spot light components.
	*/
	CATALYST_RESTRICTED static SpotLightComponent* GetSpotLightEntitySpotLightComponents() CATALYST_NOEXCEPT;

private:

	//The number of spot light entity components.
	static size_t numberOfSpotLightEntityComponents;

	//The spot light entity spot light components.
	static DynamicArray<SpotLightComponent> spotLightEntitySpotLightComponents;

};