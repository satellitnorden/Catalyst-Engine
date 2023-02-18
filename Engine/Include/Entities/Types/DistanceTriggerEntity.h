#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class DistanceTriggerEntity : public Entity
{

public:

	//Enumeration covering all states.
	enum class State : uint8
	{
		UNENTERED,
		ENTERED
	};

	//Type aliases.
	using EnterFunction = void(*)();
	using ExitFunction = void(*)();

	/*
	*	Default constructor.
	*/
	DistanceTriggerEntity() NOEXCEPT;

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
	*	Returns the world transform.
	*/
	RESTRICTED NO_DISCARD Matrix4x4 *const RESTRICT GetWorldTransform() NOEXCEPT;

};