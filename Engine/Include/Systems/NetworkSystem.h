#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/System.h>

class NetworkSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		NetworkSystem,
		SYSTEM_INITIALIZE()
	);

	/*
	*	Default constructor.
	*/
	NetworkSystem() NOEXCEPT
	{

	}

private:

	/*
	*	Initializes the network subsystem.
	*/
	void SubInitialize() NOEXCEPT;

};