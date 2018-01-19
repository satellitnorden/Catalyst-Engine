//Header file.
#include <DailyGroupQuest.h>

/*
*	Default constructor.
*/
DailyGroupQuest::DailyGroupQuest() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
DailyGroupQuest::~DailyGroupQuest() CATALYST_NOEXCEPT
{

}

/*
*	Carries out this daily quest.
*/
void DailyGroupQuest::CarryOut() CATALYST_NOEXCEPT
{
	//If this daily quest is not available, just return.
	DailyGroupQuestCompletionState currentQuestCompletionState = questCompletionState.load();

	if (currentQuestCompletionState != DailyGroupQuestCompletionState::Available && currentQuestCompletionState != DailyGroupQuestCompletionState::InProgress)
		return;

	//Mark this daily group quest as in progress.
	questCompletionState.store(DailyGroupQuestCompletionState::InProgress);

	//Join in on the fun!
	size_t myContainerIndex = currentContainerIndex.fetch_add(1);

	while (myContainerIndex < containerSize)
	{
		//Get the element at your container index!
		void *CATALYST_RESTRICT myElement = static_cast<void *CATALYST_RESTRICT>(static_cast<byte *CATALYST_RESTRICT>(container) + (objectSize * myContainerIndex));

		//Execute the function with my element!
		function(myElement);

		//Aquire my next container index!
		myContainerIndex = currentContainerIndex.fetch_add(1);
	}

	//Report that you have done your part in the completion of this daily group quest!
	const uint8 myCompletionRank = currentNumberOfCompletions.fetch_add(1);

	//If I finished last, mark this daily group quest as completed!
	if (myCompletionRank == numberOfCompletionsRequired)
	{
		questCompletionState.store(DailyGroupQuestCompletionState::Complete);
	}
}