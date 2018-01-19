//Header file.
#include <DailyQuest.h>

/*
*	Default constructor.
*/
DailyQuest::DailyQuest() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
DailyQuest::~DailyQuest() CATALYST_NOEXCEPT
{

}

/*
*	Carries out this daily quest.
*/
void DailyQuest::CarryOut() CATALYST_NOEXCEPT
{
	//If this daily quest is not available, just return.
	DailyQuestCompletionState currentQuestCompletionState = questCompletionState.load();

	if (currentQuestCompletionState != DailyQuestCompletionState::Available)
		return;

	//Try to begin this daily quest.
	if (questCompletionState.compare_exchange_strong(currentQuestCompletionState, DailyQuestCompletionState::InProgress))
	{
		//This daily quest can be carried out!
		function(arguments);

		questCompletionState = DailyQuestCompletionState::Complete;
	}
}