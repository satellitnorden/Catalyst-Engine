#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

class ClairvoyantGameSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantGameSystem);

	/*
	*	Default constructor.
	*/
	ClairvoyantGameSystem() NOEXCEPT { }

	/*
	*	Initializes the Clairvoyant game system.
	*/
	void InitializeSystem() NOEXCEPT;

};