//Header file.
#include <Multithreading/DailyQuest.h>

/*
*	Default constructor.
*/
DailyQuest::DailyQuest() NOEXCEPT
{

}

/*
*	Default destructor.
*/
DailyQuest::~DailyQuest() NOEXCEPT
{

}

/*
*	Carries out this daily quest. Returns whether or not the quest was actually carried out.
*/
bool DailyQuest::CarryOut() NOEXCEPT
{
	//If this daily quest is not available, just return.
	DailyQuestCompletionState currentQuestCompletionState = questCompletionState.load();

	if (currentQuestCompletionState != DailyQuestCompletionState::Available)
		return false;

	//Try to begin this daily quest.
	if (questCompletionState.compare_exchange_strong(currentQuestCompletionState, DailyQuestCompletionState::InProgress))
	{
		//This daily quest can be carried out!
		function(arguments);

		questCompletionState = DailyQuestCompletionState::Complete;
	}

	return true;
}