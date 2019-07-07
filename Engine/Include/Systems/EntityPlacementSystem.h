#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

class EntityPlacementSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntityPlacementSystem);

	/*
	*	Default constructor.
	*/
	EntityPlacementSystem() NOEXCEPT
	{

	}

	/*
	*	Updates the entity placement system during the sequential update phase.
	*/
	void SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

};