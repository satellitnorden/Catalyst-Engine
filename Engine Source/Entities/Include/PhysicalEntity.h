#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <ComponentManager.h>

//Entities.
#include <Entity.h>

//Graphics.
#include <PhysicalModel.h>

//Math.
#include <Matrix4.h>

//Vulkan.
#include <VulkanDescriptorSet.h>

//Forward declarations.
class CameraEntity;
class VulkanUniformBuffer;

class PhysicalEntity : public Entity
{

public:

	//Universal container of all physical entities.
	static DynamicArray<PhysicalEntity *CATALYST_RESTRICT> physicalEntities;

	/*
	*	Default constructor.
	*/
	PhysicalEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~PhysicalEntity() CATALYST_NOEXCEPT;

	/*
	*	Initializes this physical entity.
	*/
	void Initialize(const PhysicalModel &newModel) CATALYST_NOEXCEPT;

	/*
	*	Updates the model matrix.
	*/
	void UpdateModelMatrix() CATALYST_NOEXCEPT;

	/*
	*	Returns the model extent.
	*/
	float GetModelExtent() const CATALYST_NOEXCEPT { return modelExtent; }

	/*
	*	Returns the uniform buffer.
	*/
	CATALYST_RESTRICTED const VulkanUniformBuffer* GetUniformBuffer() const CATALYST_NOEXCEPT { return uniformBuffer; }

	/*
	*	Sets the uniform buffer.
	*/
	void SetUniformBuffer(VulkanUniformBuffer *CATALYST_RESTRICT newUniformBuffer) CATALYST_NOEXCEPT { uniformBuffer = newUniformBuffer; }

	/*
	*	Sets the index for the physical graphics component.
	*/
	void SetPhysicalGraphicsComponent(const size_t newPhysicalGraphicsComponent) CATALYST_NOEXCEPT { physicalGraphicsComponent = newPhysicalGraphicsComponent; }

	/*
	*	Sets whether or not this physical entity is in the view frustum.
	*/
	void SetIsInViewFrustum(const bool newIsInViewFrustum) CATALYST_NOEXCEPT { ComponentManager::GetPhysicalGraphicsComponentNonConst(physicalGraphicsComponent).isInViewFrustum.store(newIsInViewFrustum); }

	/*
	*	Returns whether or not this physical entity is in the view frustum.
	*/
	bool IsInViewFrustum() const CATALYST_NOEXCEPT { return ComponentManager::GetPhysicalGraphicsComponentConst(physicalGraphicsComponent).isInViewFrustum.load(); }

private:

	//The model matrix.
	Matrix4 modelMatrix;

	//The model extent.
	float modelExtent;

	//The uniform buffer.
	VulkanUniformBuffer *CATALYST_RESTRICT uniformBuffer;

	//The index for the physical graphics component.
	size_t physicalGraphicsComponent;

};