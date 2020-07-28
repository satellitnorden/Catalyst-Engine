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

class StaticModelEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	StaticModelEntity() NOEXCEPT;

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
	*	Returns the initialization data required to duplicate this entity.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return nullptr;
	}

	/*
	*	Returns the model resource.
	*/
	NO_DISCARD ResourcePointer<ModelResource> GetModelResource() const NOEXCEPT;

	/*
	*	Returns the material resources.
	*/
	NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &GetMaterialResources() const NOEXCEPT;

	/*
	*	Returns the level of detail index at the given mesh index.
	*/
	NO_DISCARD uint64 GetLevelOfDetailindex(const uint64 mesh_index) const NOEXCEPT;

	/*
	*	Returns the world transform.
	*/
	RESTRICTED NO_DISCARD const WorldTransform *const RESTRICT GetWorldTransform() const NOEXCEPT;

	/*
	*	Returns the model space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Returns the world space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT;

};