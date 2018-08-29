#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Entity.
#include <Entities/Entity.h>

//Forward declarations.
class Matrix4;
class PhysicalModel;
class PhysicalMaterial;

class InstancedPhysicalEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(InstancedPhysicalEntity);

	/*
	*	Default constructor.
	*/
	InstancedPhysicalEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~InstancedPhysicalEntity() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() const NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() const NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() const NOEXCEPT final override;

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

	/*
	*	Initializes this instanced physical entity.
	*/
	void Initialize(const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) NOEXCEPT;

};