#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

class WorldArchitect final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldArchitect);

	/*
	*	Default constructor.
	*/
	WorldArchitect() NOEXCEPT { }

	/*
	*	Initializes the world architect.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Initializes the vegetation.
	*/
	void InitializeVegetation();

};