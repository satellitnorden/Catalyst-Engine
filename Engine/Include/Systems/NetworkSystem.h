#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class NetworkSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(NetworkSystem);

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