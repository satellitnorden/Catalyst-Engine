#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

class InstancedStaticModelEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	InstancedStaticModelEntity() NOEXCEPT;

	/*
	*	Returns the preprocessing parameters.
	*/
	FORCE_INLINE void GetPreprocessingParameters(EntityPreprocessingParameters* const RESTRICT parameters) NOEXCEPT
	{

	}

	/*
	*	Preprocesses this entity.
	*/
	FORCE_INLINE void Preprocess(EntityInitializationData* const RESTRICT data) NOEXCEPT
	{

	}

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

};