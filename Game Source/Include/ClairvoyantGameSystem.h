#pragma once

//Engine core.
#include <EngineCore.h>

//Clairvoyant.
#include <EntitySpawner.h>
#include <WorldArchitect.h>

//Forward declarations.
class ClairvoyantPlayer;

class ClairvoyantGameSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantGameSystem);

	/*
	*	Default constructor.
	*/
	ClairvoyantGameSystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~ClairvoyantGameSystem() CATALYST_NOEXCEPT;

	/*
	*	Initializes the game system.
	*/
	void InitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the game system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) CATALYST_NOEXCEPT;

	/*
	*	Releases the game system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

private:

	//The player.
	ClairvoyantPlayer *player{ nullptr };

	//The entity spawner.
	EntitySpawner entitySpawner{ };

	//The world architect.
	WorldArchitect worldArchitect{ };

};