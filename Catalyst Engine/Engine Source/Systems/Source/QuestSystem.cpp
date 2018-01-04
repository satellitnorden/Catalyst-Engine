//Header file.
#include <QuestSystem.h>

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
	numberOfAdventurers = numberOfHardwareThreads * 2;

	//Initialize the quests queue.
	questsQueue.Initialize(256 + static_cast<size_t>(numberOfAdventurers));

	//Create the proper number of adventurers.
	adventurers.Resize(numberOfAdventurers);
	threads.Reserve(numberOfAdventurers);

	//Fire off all adventurer threads.
	for (auto &adventurer : adventurers)
	{
		threads.EmplaceUnsafe([&]()
		{
			adventurer.Update();
		});
	}
}

/*
*	Updates the system synchronously.
*/
void QuestSystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Remove completed quests.
	while (!quests.empty() && quests[0].GetCompletionState() == QuestCompletionState::Complete)
		quests.pop_front();
}

/*
*	Releases the system.
*/
void QuestSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	//Tell all adventurers to stop carrying out their quests.
	for (auto &adventurer : adventurers)
	{
		adventurer.StopCarryingOutQuests();
	}

	//Join all adventurer threads.
	for (auto &thread : threads)
	{
		thread.join();
	}
}

/*
*	Carries out a quest and returns a handle to the quest.
*/
QuestLog QuestSystem::CarryOutQuest(std::function<void()> &&function) CATALYST_NOEXCEPT
{
	//Add the new quest to the quests container.
	quests.emplace_back(std::move(function));

	//Add a pointer to the new quest to the quests queue.
	questsQueue.Push(&quests.back());

	return QuestLog(&quests.back());
}

/*
*	Returns a quest from the quest queue.
*/
CATALYST_RESTRICTED Quest* QuestSystem::GetNewQuest() CATALYST_NOEXCEPT
{
	Quest *CATALYST_RESTRICT newQuest = nullptr;

	questsQueue.PopIfNotEmpty(newQuest);

	return newQuest;
}