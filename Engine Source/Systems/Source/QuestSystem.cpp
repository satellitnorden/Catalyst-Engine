//Header file.
#include <QuestSystem.h>

//Systems.
#include <EngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(QuestSystem);

/*
*	Default constructor.
*/
QuestSystem::QuestSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
QuestSystem::~QuestSystem() CATALYST_NOEXCEPT
{

}

/*
*	Initializes the quest system.
*/
void QuestSystem::InitializeSystem() CATALYST_NOEXCEPT
{
	//Find out how many hardware threads there is.
	uint8 numberOfHardwareThreads = std::thread::hardware_concurrency();

	//In the rare case that the number of hardware threads cannot be retrieved, default to 8.
	if (numberOfHardwareThreads == 0)
		numberOfHardwareThreads = 8;

	//Set the number of adventurers.
	numberOfAdventurers = numberOfHardwareThreads - (numberOfHardwareThreads / 4);

	//Kick off all adventurer threads.
	adventurerThreads.Reserve(numberOfAdventurers);

	for (uint8 i = 0; i < numberOfAdventurers; ++i)
	{
		adventurerThreads.EmplaceUnsafe(std::move(std::thread(&QuestSystem::ExecuteAdventurer, this)));
	}
}

/*
*	Updates the system synchronously.
*/
void QuestSystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Nothing to do here yet.
}

/*
*	Releases the system.
*/
void QuestSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	//Join all adventurer threads.
	for (auto &adventurerThread : adventurerThreads)
	{
		adventurerThread.join();
	}
}

/*
*	Registers a daily quest.
*/
void QuestSystem::RegisterDailyQuest(const DailyQuests dailyQuest, DailyQuestFunction newFunction, void *CATALYST_RESTRICT arguments) CATALYST_NOEXCEPT
{
	dailyQuests[static_cast<uint8>(dailyQuest)].SetFunction(newFunction);
	dailyQuests[static_cast<uint8>(dailyQuest)].SetArguments(arguments);
}

/*
*	Registers a daily group quest.
*
*	dailyGroupQuest - The daily group quest that is being registered.
*	objectSize = The size in bytes of the objects contained in the container.
*	function = Pointer to a function that will take on 'void *CATALYST_RESTRICT' parameter - this corresponds to one element in the container.
*/
void QuestSystem::RegisterDailyGroupQuest(const DailyGroupQuests dailyGroupQuest, DailyGroupQuestFunction function) CATALYST_NOEXCEPT
{
	dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetFunction(function);
	dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetNumberOfCompletionsRequired(numberOfAdventurers);
}

/*
*	Carries out a daily quest.
*/
void QuestSystem::CarryOutDailyQuest(const DailyQuests dailyQuest) CATALYST_NOEXCEPT
{
	//Only carry out the daily quest if it is not available or in progress.
	DailyQuestCompletionState questCompletionState = dailyQuests[static_cast<uint8>(dailyQuest)].GetQuestCompletionState();

	if (questCompletionState == DailyQuestCompletionState::Unavailable || questCompletionState == DailyQuestCompletionState::Complete)
	{
		dailyQuests[static_cast<uint8>(dailyQuest)].SetQuestCompletionState(DailyQuestCompletionState::Available);
	}
}

/*
*	Carries out a daily group quest.
*/
void QuestSystem::CarryOutDailyGroupQuest(const DailyGroupQuests dailyGroupQuest, void *CATALYST_RESTRICT container, const size_t containerSize, const size_t objectSize) CATALYST_NOEXCEPT
{
	//Only carry out the daily group quest if it is not available or in progress.
	DailyGroupQuestCompletionState questCompletionState = dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].GetQuestCompletionState();

	if (questCompletionState == DailyGroupQuestCompletionState::Unavailable || questCompletionState == DailyGroupQuestCompletionState::Complete)
	{
		dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetContainer(container);
		dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetContainerSize(containerSize);
		dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetObjectSize(objectSize);
		dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetCurrentContainerIndex(0);
		dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetCurrentNumberOfCompletions(0);
		dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].SetQuestCompletionState(DailyGroupQuestCompletionState::Available);
	}
}

/*
*	Waits for a daily group quest to begin.
*/
void QuestSystem::WaitForDailyGroupQuest(const DailyGroupQuests dailyGroupQuest) const CATALYST_NOEXCEPT
{
	while (dailyGroupQuests[static_cast<uint8>(dailyGroupQuest)].GetQuestCompletionState() != DailyGroupQuestCompletionState::Complete)
	{
		std::this_thread::yield();
	}
}

/*
*	Executes an adventurer.
*/
void QuestSystem::ExecuteAdventurer() CATALYST_NOEXCEPT
{
	while (!EngineSystem::Instance->ShouldTerminate())
	{
		//Carry out all daily quests.
		for (uint8 i = 0; i < static_cast<uint8>(DailyQuests::NumberOfDailyQuests); ++i)
		{
			dailyQuests[i].CarryOut();
		}

		//Carry out all daily group quests.
		for (uint8 i = 0; i < static_cast<uint8>(DailyGroupQuests::NumberOfDailyGroupQuests); ++i)
		{
			dailyGroupQuests[i].CarryOut();
		}
	}
}