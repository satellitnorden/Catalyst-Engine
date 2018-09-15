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
	RESTRICTED NO_DISCARD AxisAlignedBoundingBox *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Returns the axis aligned bounding box for this dynamic physical entity in world space.
	*/
	RESTRICTED NO_DISCARD AxisAlignedBoundingBox *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Callback for when this entity is moved.
	*/
	void OnMove() NOEXCEPT final override
	{
		//Update the world space axis - aligned bounding box.
		UpdateWorldSpaceAxisAlignedBoundingBox();
	}

protected:

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3 *const RESTRICT GetScaleInternal() NOEXCEPT final override;

private:

	/*
	*	Updates the world space axis-aligned bounding box.
	*/
	void UpdateWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;

};