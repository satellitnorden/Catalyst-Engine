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
	WorldArchitect() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.	
	*/
	~WorldArchitect() CATALYST_NOEXCEPT;

	/*
	*	Initializes the world architect.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Updates the world architects.
	*/
	void Update(const float deltaTime) CATALYST_NOEXCEPT;

private:

	//The sun.
	DirectionalLightEntity *CATALYST_RESTRICT sun;

};