//Header file.
#include <Main/ClairvoyantGameSystem.h>

//Clairvoyant.
#include <Player/ClairvoyantPlayer.h>

//Managers.
#include <Managers/EnvironmentManager.h>

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
	//Set the environment materials/blend.
	EnvironmentManager::Instance->SetNightEnvironmentMaterial(RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Night));
	EnvironmentManager::Instance->SetDayEnvironmentMaterial(RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Day));
	EnvironmentManager::Instance->SetEnvironmentBlend(1.0f);
#if defined(CATALYST_ENABLE_OCEAN)
	EnvironmentManager::Instance->SetOceanMaterial(RenderingSystem::Instance->GetCommonOceanMaterial(RenderingSystem::CommonOceanMaterial::Ocean));
#endif

	//Initialize the player.
	ClairvoyantPlayer::Instance->Initialize();

}