#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Forward declarations.
class DirectionalLightEntity;

class WorldArchitect final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldArchitect);

	/*
	*	Initializes the world architect.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the world architects.
	*/
	void Update(const float deltaTime) NOEXCEPT;

};