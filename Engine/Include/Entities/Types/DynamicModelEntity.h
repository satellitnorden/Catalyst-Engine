#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Physics.
#include <Physics/Native/ModelCollisionConfiguration.h>
#include <Physics/Native/ModelSimulationConfiguration.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>
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
	*	Returns the initialization data required to duplicate this entity.
	*/
	RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT;

	/*
	*	Returns the model resource.
	*/
	NO_DISCARD ResourcePointer<ModelResource> GetModelResource() const NOEXCEPT;

	/*
	*	Sets the model resource.
	*/
	void SetModelResource(const ResourcePointer<ModelResource> resource) NOEXCEPT;

	/*
	*	Returns the material resources.
	*/
	NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &GetMaterialResources() const NOEXCEPT;

	/*
	*	Sets the material resource at the given index.
	*/
	void SetMaterialResource(const uint8 index, const ResourcePointer<MaterialResource> resource) NOEXCEPT;

	/*
	*	Returns the level of detail index at the given mesh index.
	*/
	NO_DISCARD uint64 GetLevelOfDetailindex(const uint64 mesh_index) const NOEXCEPT;

	/*
	*	Returns the world transform.
	*/
	RESTRICTED NO_DISCARD const WorldTransform *const RESTRICT GetWorldTransform() const NOEXCEPT;

	/*
	*	Sets the world transform.
	*/
	void SetWorldTransform(const WorldTransform &new_world_transform) NOEXCEPT;

	/*
	*	Returns the model space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3D *const RESTRICT GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Returns the world space axis aligned bounding box.
	*/
	RESTRICTED NO_DISCARD const WorldSpaceAxisAlignedBoundingBox3D *const RESTRICT GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT;

	/*
	*	Returns the model collision configuration.
	*/
	NO_DISCARD const ModelCollisionConfiguration &GetModelCollisionConfiguration() const NOEXCEPT;

	/*
	*	Sets the model collision configuration.
	*/
	void SetModelCollisionConfiguration(const ModelCollisionConfiguration &value) NOEXCEPT;

	/*
	*	Returns the model simulation configuration.
	*/
	RESTRICTED NO_DISCARD const ModelSimulationConfiguration *const RESTRICT GetModelSimulationConfiguration() const NOEXCEPT;

private:

	/*
	*	Updates the world space axis aligned bounding box.
	*/
	void UpdateWorldSpaceAxisAlignedBoundingBox() NOEXCEPT;
};