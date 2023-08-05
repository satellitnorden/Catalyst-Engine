#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Core/EntityCore.h>
#include <Entities/Creation/EntityInitializationData.h>
#include <Entities/Creation/EntityPreprocessingParameters.h>

class Entity
{

public:

	//The type.
	EntityType _Type{ EntityType::NumberOfEntityTypes };

	//Denotes whether or not this entity is initialized.
	bool _Initialized{ false };

	//The components index for this entity.
	uint64 _ComponentsIndex;

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
	bool ShouldAutomaticallyTerminate() const NOEXCEPT;

	/*
	*	Returns the initialization data required to duplicate this entity.
	*/
	RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT;

};