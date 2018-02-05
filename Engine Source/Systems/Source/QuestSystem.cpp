//Header file.
#include <QuestSystem.h>

//Systems.
#include <EngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(QuestSystem);

/*
*	Default constructor.
*/
QuestSystem::QuestSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
QuestSystem::~QuestSystem() NOEXCEPT
{

}

/*
*	Initializes the quest system.
*/
void QuestSystem::InitializeSystem() NOEXCEPT
{
	//Find out how many hardware threads there is.
	uint8 numberOfHardwareThreads = std::thread::hardware_concurrency();

	//In the rare case that the number of hardware threads cannot be retrieved, default to 8.
	if (numberOfHardwareThreads == 0)
		numberOfHardwareThreads = 8;

	//Set the number of adventurers.
	numberOfAdventurers = numberOfHardwareThreads;

	//Kick off all adventurer threads.
	adventurerThreads.Reserve(numberOfAdventurers);

	for (uint8 i = 0; i < numberOfAdventurers; ++i)
	{
		adventurerThreads.EmplaceFast(std::move(std::thread(&QuestSystem::ExecuteAdventurer, this)));
	}
}

/*
*	Releases the system.
*/
void QuestSystem::ReleaseSystem() NOEXCEPT
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
void QuestSystem::RegisterDailyQuest(const DailyQuests dailyQuest, DailyQuestFunction newFunction) NOEXCEPT
{
	dailyQuests[static_cast<uint8>(dailyQuest)].SetFunction(newFunction);
}

/*
*	Carries out a daily quest.
*/
void QuestSystem::CarryOutDailyQuest(const DailyQuests dailyQuest, void *RESTRICT arguments) NOEXCEPT
{
	//Only carry out the daily quest if it is not available or in progress.
	DailyQuestCompletionState questCompletionState = dailyQuests[static_cast<uint8>(dailyQuest)].GetQuestCompletionState();

	if (questCompletionState == DailyQuestCompletionState::Unavailable || questCompletionState == DailyQuestCompletionState::Complete)
	{
		dailyQuests[static_cast<uint8>(dailyQuest)].SetArguments(arguments);
		dailyQuests[static_cast<uint8>(dailyQuest)].SetQuestCompletionState(DailyQuestCompletionState::Available);
	}
}

/*
*	Waits for a daily quest to be completed.
*/
void QuestSystem::WaitForDailyQuest(const DailyQuests dailyQuest) const NOEXCEPT
{
	while (dailyQuests[static_cast<uint8>(dailyQuest)].GetQuestCompletionState() != DailyQuestCompletionState::Complete)
	{
		std::this_thread::yield();
	}
}

/*
*	Executes an adventurer.
*/
void QuestSystem::ExecuteAdventurer() NOEXCEPT
{
	while (!EngineSystem::Instance->ShouldTerminate())
	{
		//Keeep track of how many quests were actually carried out.
		uint8 carriedOutQuests{ 0 };

		//Carry out all daily quests.
		for (uint8 i = 0; i < static_cast<uint8>(DailyQuests::NumberOfDailyQuests); ++i)
		{
			carriedOutQuests += dailyQuests[i].CarryOut() ? 1 : 0;
		}

		//If there were no quests that were carried out, then pave the way for other hypothetical threads.
		if (carriedOutQuests == 0)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}
}