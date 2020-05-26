#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldTransform.h>

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
	*	Returns the model resources.
	*/
	NO_DISCARD ResourcePointer<ModelResource> GetModelResource() const NOEXCEPT;

	/*
	*	Returns the material resources.
	*/
	NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &GetMaterialResources() const NOEXCEPT;

	/*
	*	Returns the world transform.
	*/
	RESTRICTED NO_DISCARD const WorldTransform *const RESTRICT GetWorldTransform() const NOEXCEPT;

	/*
	*	Returns the world transform. Assumes the world transform will be modified, and will notify relevant systems.
	*/
	RESTRICTED NO_DISCARD WorldTransform *const RESTRICT ModifyWorldTransform() NOEXCEPT;

	/*
	*	Returns the model space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Returns the world space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT;

};