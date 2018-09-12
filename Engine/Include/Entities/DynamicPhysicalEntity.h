#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Entity.h>

class AxisAlignedBoundingBox;

class DynamicPhysicalEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	DynamicPhysicalEntity() NOEXCEPT;

	/*
	*	Returns the axis aligned bounding box for this dynamic physical entity in model space.
	*/
	NO_DISCARD RESTRICTED const AxisAlignedBoundingBox *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Returns the axis aligned bounding box for this dynamic physical entity in world space.
	*/
	NO_DISCARD RESTRICTED const AxisAlignedBoundingBox *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT;

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