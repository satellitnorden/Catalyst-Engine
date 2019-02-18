#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class AxisAlignedBoundingBox;

class DynamicPhysicalEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	DynamicPhysicalEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(const EntityInitializationData *const RESTRICT data) NOEXCEPT
	{

	}

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT
	{

	}

	/*
	*	Returns the axis aligned bounding box for this dynamic physical entity in model space.
	*/
	RESTRICTED NO_DISCARD AxisAlignedBoundingBox *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Returns the axis aligned bounding box for this dynamic physical entity in world space.
	*/
	RESTRICTED NO_DISCARD AxisAlignedBoundingBox *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT GetPositionInternal() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT GetRotationInternal() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT GetScaleInternal() NOEXCEPT final override;

	/*
	*	Callback for when this entity is moved.
	*/
	void OnMove() NOEXCEPT final override
	{
		//Update the world space axis - aligned bounding box.
		UpdateWorldSpaceAxisAlignedBoundingBox();
	}

private:

	/*
	*	Updates the world space axis-aligned bounding box.
	*/
	void UpdateWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;

};