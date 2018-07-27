#pragma once

//Core.
#include <Core/EngineCore.h>

//Clairvoyant.
#include <Sound/ClairvoyantSoundDirector.h>
#include <World/WorldArchitect.h>

//Preprocessor defines.
#define USE_SOAK_CAMERA false

//Forward declarations.
class ClairvoyantPlayer;
class SoakCamera;

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

#if USE_SOAK_CAMERA
	//The soak camera.
	SoakCamera *RESTRICT soakCamera;
#else
	//The player.
	ClairvoyantPlayer *RESTRICT player;
#endif

};