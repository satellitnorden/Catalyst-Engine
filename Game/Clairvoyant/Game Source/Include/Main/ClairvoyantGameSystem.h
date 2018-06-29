#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Clairvoyant.
#include <Sound/ClairvoyantSoundDirector.h>
#include <World/WorldArchitect.h>

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
	ClairvoyantGameSystem() NOEXCEPT = default;

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
	ClairvoyantPlayer *RESTRICT player;

};