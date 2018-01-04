#pragma once

//Engine core.
#include <EngineCore.h>

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
	~PhysicalEntity() CATALYST_NOEXCEPT;

	/*
	*	Initializes this physical entity.
	*/
	void Initialize(const PhysicalModel &newModel) CATALYST_NOEXCEPT;

	/*
	*	Updates the model matrix.
	*/
	void UpdateModelMatrix() CATALYST_NOEXCEPT;

	/*
	*	Returns the model.
	*/
	CATALYST_FORCE_INLINE const PhysicalModel& GetModel() const CATALYST_NOEXCEPT { return model; }

	/*
	*	Returns the uniform buffer.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED VulkanUniformBuffer* GetUniformBuffer() const CATALYST_NOEXCEPT { return uniformBuffer; }

	/*
	*	Returns the descriptor set.
	*/
	CATALYST_FORCE_INLINE const VulkanDescriptorSet& GetDescriptorSet() const CATALYST_NOEXCEPT { return descriptorSet; }

	/*
	*	Sets whether or not this physical entity is in the view frustum.
	*/
	CATALYST_FORCE_INLINE void SetIsInViewFrustum(const bool newIsInViewFrustum) CATALYST_NOEXCEPT { isInViewFrustum.store(newIsInViewFrustum); }

	/*
	*	Returns whether or not this physical entity is in the view frustum.
	*/
	CATALYST_FORCE_INLINE bool IsInViewFrustum() const CATALYST_NOEXCEPT { return isInViewFrustum; }

private:

	//The model matrix.
	Matrix4 modelMatrix{ };

	//The model.
	PhysicalModel model{ };

	//The uniform buffer.
	VulkanUniformBuffer *CATALYST_RESTRICT uniformBuffer{ nullptr };

	//The descriptor set.
	VulkanDescriptorSet descriptorSet{ };

	//Denotes whether or not this physical entity is in the view frustum.
	std::atomic<bool> isInViewFrustum{ true };

};