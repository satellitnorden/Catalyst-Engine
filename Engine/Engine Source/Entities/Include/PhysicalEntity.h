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

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(PhysicalEntity);

	/*
	*	Default constructor.
	*/
	PhysicalEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~PhysicalEntity() NOEXCEPT;

	/*
	*	Initializes this physical entity.
	*/
	void Initialize(const PhysicalModel &newModel) NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) NOEXCEPT final override;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &scaleVector) NOEXCEPT final override;

};