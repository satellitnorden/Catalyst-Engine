#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class TerrainEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	TerrainEntity() NOEXCEPT;

	/*
	*	Returns the preprocessing parameters.
	*/
	void GetPreprocessingParameters(EntityPreprocessingParameters *const RESTRICT parameters) NOEXCEPT;

	/*
	*	Preprocesses this entity.
	*/
	void Preprocess(EntityInitializationData *const RESTRICT data) NOEXCEPT;

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

};