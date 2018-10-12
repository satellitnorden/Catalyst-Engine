//Header file.
#include <Main/ClairvoyantGameSystem.h>

//Clairvoyant.
#include <Player/ClairvoyantPlayer.h>
#include <Player/SoakCamera.h>
#include <World/ClairvoyantWorldArchitect.h>
#include <World/TimeOfDaySystem.h>

//Entities.
#include <Entities/ParticleSystemEntity.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantGameSystem);

/*
*	Initializes the Clairvoyant game system.
*/
void ClairvoyantGameSystem::InitializeSystem() NOEXCEPT
{
	//Load the resource collection.
	ResourceLoader::LoadResourceCollection("..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection.crc");

	//Set the environment materials/blend.
#if defined(CATALYST_ENABLE_OCEAN)
	EnvironmentManager::Instance->SetOceanMaterial(ResourceLoader::GetOceanMaterial(HashString("DefaultOceanMaterial")));
#endif

	//Initialize the world architect.
	ClairvoyantWorldArchitect::Instance->Initialize();

	//Initialize the player.
	ClairvoyantPlayer::Instance->Initialize();

	//Initialize the soak camera.
	//SoakCamera::Instance->Initialize();

	//Initialize the time of day system.
	TimeOfDaySystem::Instance->Initialize();
}