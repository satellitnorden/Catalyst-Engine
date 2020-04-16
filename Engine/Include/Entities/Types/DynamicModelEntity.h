#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Rendering.
#include <Rendering/Native/Material.h>

class DynamicModelEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	DynamicModelEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	bool ShouldAutomaticallyTerminate() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return false;
	}

	/*
	*	Returns the world transform.
	*/
	RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT GetWorldTransform() const NOEXCEPT;

	/*
	*	Returns the world transform. Assumes the world transform will be modified, and will notify relevant systems.
	*/
	RESTRICTED NO_DISCARD Matrix4x4 *const RESTRICT ModifyWorldTransform() NOEXCEPT;

	/*
	*	Returns the model space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Returns the world space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Enables highlight on this model entity.
	*/
	void EnableHighlight(const Vector3<float> &color, const float32 strength) NOEXCEPT;

	/*
	*	Returns if this model entity is highlighted or not.
	*/
	NO_DISCARD bool IsHighlighted() const NOEXCEPT;

	/*
	*	Sets the highlight color on this model entity.
	*/
	void SetHighlightColor(const Vector3<float> &color) NOEXCEPT;

	/*
	*	Sets the highlight strength on this model entity.
	*/
	void SethighlightStrength(const float32 strength) NOEXCEPT;

	/*
	*	Disables highlight on this model entity.
	*/
	void DisableHighlight() NOEXCEPT;

};