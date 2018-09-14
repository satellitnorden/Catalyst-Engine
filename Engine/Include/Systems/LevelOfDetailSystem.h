#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

class LevelOfDetailSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LevelOfDetailSystem);

	/*
	*	Default constructor.
	*/
	LevelOfDetailSystem() NOEXCEPT { }

};