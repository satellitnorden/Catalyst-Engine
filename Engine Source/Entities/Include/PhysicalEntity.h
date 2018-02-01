#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

//Forward declarations.
class PhysicalModel;
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
	*	Returns the position of this entity.
	*/
	Vector3& GetPosition() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	Vector3& GetRotation() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	Vector3& GetScale() CATALYST_NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) CATALYST_NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT final override;

	/*
	*	Scales this entity.
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

	//The uniform buffer.
	VulkanUniformBuffer *CATALYST_RESTRICT uniformBuffer;

};