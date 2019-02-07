#if defined(CATALYST_PHYSICS_CATALYST)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Pointers/UniquePointer.h>

class CatalystPhysicsSystem final
{

public:

	//Singelton declaration.
	DECLARE_SINGLETON(CatalystPhysicsSystem);

	/*
	*	Default constructor.
	*/
	CatalystPhysicsSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the Catalyst physics system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the Catalyst physics system synchronously.
	*/
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Releases the Catalyst physics system.
	*/
	void ReleaseSystem() NOEXCEPT;

};
#endif