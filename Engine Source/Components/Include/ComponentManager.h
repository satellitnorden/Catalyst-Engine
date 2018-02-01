#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <FrustumCullingComponent.h>
#include <PhysicalGraphicsComponent.h>
#include <TransformComponent.h>

class ComponentManager
{

public:

	/*
	*	Returns a new components index for physical entities.
	*/
	static size_t GetNewPhysicalEntityComponentsIndex() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of physical entity components.
	*/
	static size_t GetNumberOfPhysicalEntityComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns physical entity frustum culling components.
	*/
	CATALYST_RESTRICTED static FrustumCullingComponent* GetPhysicalEntityFrustumCullingComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns physical entity graphics components.
	*/
	CATALYST_RESTRICTED static PhysicalGraphicsComponent* GetPhysicalEntityGraphicsComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns physical entity transform components.
	*/
	CATALYST_RESTRICTED static TransformComponent* GetPhysicalEntityTransformComponents() CATALYST_NOEXCEPT;

private:

	//The number of physical entity components.
	static size_t numberOfPhysicalEntityComponents;

	//The physical entity frustum culling components.
	static DynamicArray<FrustumCullingComponent> physicalEntityFrustumCullingComponents;

	//The physical entity graphics components.
	static DynamicArray<PhysicalGraphicsComponent> physicalEntityGraphicsComponents;

	//The physical entity transform components.
	static DynamicArray<TransformComponent> physicalEntityTransformComponents;

};