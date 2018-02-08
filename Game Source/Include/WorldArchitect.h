#pragma once

//Engine core.
#include <EngineCore.h>

class WorldArchitect final
{

public:

	/*
	*	Default constructor.
	*/
	WorldArchitect() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.	
	*/
	~WorldArchitect() CATALYST_NOEXCEPT;

	/*
	*	Initializes the world architect.
	*/
	void Initialize() CATALYST_NOEXCEPT;

};