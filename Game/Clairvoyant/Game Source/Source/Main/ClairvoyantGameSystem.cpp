//Header file.
#include <Main/ClairvoyantGameSystem.h>

//Clairvoyant.
#include <Debug/SoakCamera.h>
#include <Player/ClairvoyantPlayer.h>
#include <World/TimeSystem.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantGameSystem);

/*
*	Initializes the game system.
*/
void ClairvoyantGameSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the world architect.
	WorldArchitect::Instance->Initialize();

	/*
	//Create the player.
	player = EntitySystem::Instance->CreateEntity<ClairvoyantPlayer>();

	//Initialize the player.
	player->Initialize();
	*/

	//Create the soak camera.
	soakCamera = EntitySystem::Instance->CreateEntity<SoakCamera>();

	//Initialize the soak camera.
	soakCamera->Initialize();

	//Initialize the sound director.
	ClairvoyantSoundDirector::Instance->Initialize();

	//Initialize the time system.
	TimeSystem::Instance->Initialize();
}

/*
*	Updates the game system synchronously.
*/
void ClairvoyantGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	/*
	//Update the player.
	player->Update(deltaTime);
	*/

	//Update the soak camera.
	soakCamera->Update(deltaTime);

	//Update the sound director.
	ClairvoyantSoundDirector::Instance->Update();

	//Update the time system.
	TimeSystem::Instance->Update(deltaTime);

	//Update the world architect.
	WorldArchitect::Instance->Update(deltaTime);
}

/*
*	Releases the game system.
*/
void ClairvoyantGameSystem::ReleaseSystem() NOEXCEPT
{

}