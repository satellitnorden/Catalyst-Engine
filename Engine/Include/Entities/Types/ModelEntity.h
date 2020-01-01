#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Rendering.
#include <Rendering/Native/Material.h>

class ModelEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	ModelEntity() NOEXCEPT;

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
	RESTRICTED NO_DISCARD Matrix4 *const RESTRICT GetWorldTransform() NOEXCEPT;

	/*
	*	Returns the model space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Returns the world space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;

	/*
	*	Disables highlight on this model entity.
	*/
	void DisableHighlight() NOEXCEPT;

	/*
	*	Returns if this model entity is highlighted or not.
	*/
	NO_DISCARD bool IsHighlighted() const NOEXCEPT;

	/*
	*	Enables highlight on this model entity.
	*/
	void EnableHighlight(const Vector3<float> &color, const float strength) NOEXCEPT;

};