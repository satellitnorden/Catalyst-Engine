#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

//Forward declarations.
class PhysicalModel;
class VulkanUniformBuffer;

class StaticPhysicalEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	StaticPhysicalEntity() NOEXCEPT;

	/*
	*	Initializes this static physical entity.
	*/
	void Initialize(const PhysicalModel &newModel, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) NOEXCEPT;

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

};