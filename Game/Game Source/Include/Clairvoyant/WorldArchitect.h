#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class DirectionalLightEntity;

class WorldArchitect final
{

public:

	/*
	*	Default constructor.
	*/
	WorldArchitect() NOEXCEPT;

	/*
	*	Default destructor.	
	*/
	~WorldArchitect() NOEXCEPT;

	/*
	*	Initializes the world architect.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the world architects.
	*/
	void Update(const float deltaTime) NOEXCEPT;

private:

	//The sun.
	DirectionalLightEntity *RESTRICT sun;

};