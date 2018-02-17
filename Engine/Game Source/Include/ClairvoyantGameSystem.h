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
	ClairvoyantGameSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~ClairvoyantGameSystem() NOEXCEPT;

	/*
	*	Initializes the game system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the game system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Releases the game system.
	*/
	void ReleaseSystem() NOEXCEPT;

private:

	//The player.
	ClairvoyantPlayer *player{ nullptr };

	//The entity spawner.
	EntitySpawner entitySpawner{ };

	//The world architect.
	WorldArchitect worldArchitect{ };

};