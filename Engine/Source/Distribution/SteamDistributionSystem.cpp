#if defined(CATALYST_DISTRIBUTION_STEAM)
//Preprocessor.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#define STEAM_ERROR_CHECK(FUNCTION) if (!(FUNCTION)) ASSERT(false, "Steam function failed!");
#else
#define STEAM_ERROR_CHECK(FUNCTION) FUNCTION;
#endif

//Header file.
#include <Systems/DistributionSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Third party.
#include <ThirdParty/steam/steam_api.h>

/*
*	Steam callback class definition.
*/
class SteamCallbacks final
{
	STEAM_CALLBACK(SteamCallbacks, OnGameOverlayActivated, GameOverlayActivated_t);
	STEAM_CALLBACK(SteamCallbacks, OnUserStatsReceived, UserStatsReceived_t);
};

/*
*	Callback for when the game overlay is activated.
*/
void SteamCallbacks::OnGameOverlayActivated(GameOverlayActivated_t *const RESTRICT value)
{
	if (value->m_bActive)
	{
		CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);
	}

	else
	{
		CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);
	}
}

/*
*	Callback for when the user stats are received.
*/
void SteamCallbacks::OnUserStatsReceived(UserStatsReceived_t *const RESTRICT value)
{
	ASSERT(value->m_eResult != EResult::k_EResultFail, "Couldn't retrieve Steam user stats!");
}

//Steam distribution system data.
namespace SteamDistributionSystemData
{

	//The Steam callbacks object.
	SteamCallbacks *RESTRICT _SteamCallbacksObject{ nullptr };

}

//Steam distribution system logic.
namespace SteamDistributionSystemLogic
{

	/*
	*	Updates the Steam distribution system.
	*/
	void Update() NOEXCEPT
	{
		//Run callbacks.
		SteamAPI_RunCallbacks();
	}

}

/*
*	Initializes the distribution system.
*/
void DistributionSystem::Initialize() NOEXCEPT
{
	//Initialize the Steam API.
	const bool succeeded{ SteamAPI_Init() };

	if (!succeeded)
	{
		//ASSERT(false, "Failed to initialize Steam!");

		return;
	}

	ASSERT(SteamUser()->BLoggedOn(), "Steam user is not logged in!");

	//Allocate/construct the Steam callbacks object.
	SteamDistributionSystemData::_SteamCallbacksObject = new (Memory::Allocate(sizeof(SteamCallbacks))) SteamCallbacks();

	//Request the current state (achievements/stats) from the server.
	if (SteamUserStats())
	{
		STEAM_ERROR_CHECK(SteamUserStats()->RequestCurrentStats());
	}

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const arguments)
	{
		SteamDistributionSystemLogic::Update();
	},
	nullptr,
	UpdatePhase::PRE,
	UpdatePhase::POST,
	false,
	false);
}

/*
*	Terminates the distribution system.
*/
void DistributionSystem::Terminate() NOEXCEPT
{
	//Destruct/free the Steam callbacks object.
	if (SteamDistributionSystemData::_SteamCallbacksObject)
	{
		SteamDistributionSystemData::_SteamCallbacksObject->~SteamCallbacks();
		Memory::Free(SteamDistributionSystemData::_SteamCallbacksObject);
	}

	//Store the achievements/stats on the server.
	if (SteamUserStats())
	{
		SteamUserStats()->StoreStats();
	}

	//Shutdown the Steam API.
	SteamAPI_Shutdown();
}

/*
*	Returns the current user name.
*/
RESTRICTED NO_DISCARD const char *const RESTRICT DistributionSystem::CurrentUserName() const NOEXCEPT
{
	if (SteamFriends())
	{
		return SteamFriends()->GetPersonaName();
	}
	
	else
	{
		return nullptr;
	}
}

/*
*	Clears an achievement, resetting it's state.
*/
void DistributionSystem::ClearAchievement(const AchievementIdentifier &identifier) const NOEXCEPT
{
	if (SteamUserStats())
	{
		STEAM_ERROR_CHECK(SteamUserStats()->ClearAchievement(identifier._Name));
	}
}

/*
*	Grants an achievement.
*/
void DistributionSystem::GrantAchievement(const AchievementIdentifier &identifier) const NOEXCEPT
{
	if (SteamUserStats())
	{
		STEAM_ERROR_CHECK(SteamUserStats()->SetAchievement(identifier._Name));
	}
}
#endif