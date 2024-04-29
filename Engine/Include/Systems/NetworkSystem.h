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
		NetworkSystem
	);

	/*
	*	Default constructor.
	*/
	NetworkSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the network system.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Initializes the network subsystem.
	*/
	void SubInitialize() NOEXCEPT;

};