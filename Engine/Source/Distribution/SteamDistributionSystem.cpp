#if defined(CATALYST_DISTRIBUTION_STEAM)
//Header file.
#include <Systems/DistributionSystem.h>

//Third party.
#include <ThirdParty/steam/steam_api.h>

/*
*	Initializes the distribution system.
*/
void DistributionSystem::Initialize() NOEXCEPT
{
	//Initialize the Steam API.
	const bool succeeded{ SteamAPI_Init() };
}

/*
*	Terminates the distribution system.
*/
void DistributionSystem::Terminate() NOEXCEPT
{
	//Shutdown the Steam API.
	SteamAPI_Shutdown();
}
#endif