//Header file.
#include <ClairvoyantGameSystem.h>

//Clairvoyant.
#include <ClairvoyantPlayer.h>

//Systems.
#include <EntitySystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantGameSystem);

/*
*	Default constructor.
*/
ClairvoyantGameSystem::ClairvoyantGameSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
ClairvoyantGameSystem::~ClairvoyantGameSystem() NOEXCEPT
{

}

/*
*	Initializes the game system.
*/
void ClairvoyantGameSystem::InitializeSystem() NOEXCEPT
{
	//Create the player.
	player = EntitySystem::Instance->CreateEntity<ClairvoyantPlayer>();

	//Initialize the player.
	player->Initialize();

	//Initialize the world architect.
	worldArchitect.Initialize();
}

/*
*	Updates the game system synchronously.
*/
void ClairvoyantGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Update the world architect.
	worldArchitect.Update(deltaTime);

	//Update the player.
	player->Update(deltaTime);

	//Update the entity spawner.
	entitySpawner.Update(deltaTime);
}

/*
*	Releases the game system.
*/
void ClairvoyantGameSystem::ReleaseSystem() NOEXCEPT
{

}