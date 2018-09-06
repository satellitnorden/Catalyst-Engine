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

protected:

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetScaleInternal() NOEXCEPT final override;

};