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
ClairvoyantGameSystem::ClairvoyantGameSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
ClairvoyantGameSystem::~ClairvoyantGameSystem() CATALYST_NOEXCEPT
{

}

/*
*	Initializes the game system.
*/
void ClairvoyantGameSystem::InitializeSystem() CATALYST_NOEXCEPT
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
void ClairvoyantGameSystem::UpdateSystemSynchronous(const float deltaTime) CATALYST_NOEXCEPT
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
void ClairvoyantGameSystem::ReleaseSystem() CATALYST_NOEXCEPT
{

}