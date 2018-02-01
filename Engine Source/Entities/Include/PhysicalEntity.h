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
	static DynamicArray<PhysicalEntity *CATALYST_RESTRICT> instances;

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
	*	Moves this physical entity.
	*/
	void Move(const Vector3 &moveVector) CATALYST_NOEXCEPT final override;

	/*
	*	Rotates this physical entity.
	*/
	void Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT final override;

	/*
	*	Scales this physical entity.
	*/
	void Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT final override;

	/*
	*	Updates the model matrix.
	*/
	void UpdateModelMatrix() CATALYST_NOEXCEPT;

	/*
	*	Returns the uniform buffer.
	*/
	CATALYST_RESTRICTED const VulkanUniformBuffer* GetUniformBuffer() const CATALYST_NOEXCEPT { return uniformBuffer; }

	/*
	*	Sets the uniform buffer.
	*/
	void SetUniformBuffer(VulkanUniformBuffer *CATALYST_RESTRICT newUniformBuffer) CATALYST_NOEXCEPT { uniformBuffer = newUniformBuffer; }

private:

	//The model matrix.
	Matrix4 modelMatrix;

	//The uniform buffer.
	VulkanUniformBuffer *CATALYST_RESTRICT uniformBuffer;

};