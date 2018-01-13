#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <Adventurer.h>
#include <AtomicQueue.h>
#include <Quest.h>
#include <QuestLog.h>

class QuestSystem
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(QuestSystem);

	/*
	*	Default constructor.
	*/
	QuestSystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~QuestSystem() CATALYST_NOEXCEPT;

	/*
	*	Initializes the quest system.
	*/
	void InitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the quest system synchronously.
	*/
	void UpdateSystemSynchronous() CATALYST_NOEXCEPT;

	/*
	*	Releases the quest system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Carries out a quest and returns a handle to the quest.
	*/
	QuestLog CarryOutQuest(std::function<void()> &&function) CATALYST_NOEXCEPT;

	/*
	*	Given an iteratable container of objects and a function taking one object of that container, do the same thing on all objects.
	*/
	template <class ObjectType>
	void QuestSystem::DoForAll(const DynamicArray<ObjectType> &container, std::function<void(ObjectType)> &&function) CATALYST_NOEXCEPT;

	/*
	*	Returns a quest from the quest queue.
	*/
	CATALYST_RESTRICTED Quest* GetNewQuest() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of adventurers.
	*/
	CATALYST_FORCE_INLINE uint8 GetNumberOfAdventurers() const CATALYST_NOEXCEPT { return numberOfAdventurers; }

private:

	//Denotes the number of adventurers.
	uint8 numberOfAdventurers;

	//Container for all threads.
	DynamicArray<std::thread> threads;

	//Container for all adventurers.
	DynamicArray<Adventurer> adventurers;

	//Container for quests that are waiting to be carried out.
	std::deque<Quest> quests;

	//The atomic queue for the quests that are waiting to be carried out.
	AtomicQueue<Quest *CATALYST_RESTRICT> questsQueue;

};

//////////////////////////////////////////////////////////////////////
//////////			Templated function definitions.			//////////
//////////////////////////////////////////////////////////////////////

/*
*	Given an iteratable container of objects and a function taking one object of that container, do the same thing on all objects.
*/
template <class ObjectType>
void QuestSystem::DoForAll(const DynamicArray<ObjectType> &container, std::function<void(ObjectType)> &&function) CATALYST_NOEXCEPT
{
	std::atomic<size_t> counter = 0;
	DynamicArray<QuestLog> questLogs;
	questLogs.Reserve(numberOfAdventurers);

	for (uint8 i = 0; i < numberOfAdventurers; ++i)
	{
		questLogs.EmplaceUnsafe(CarryOutQuest([&]()
		{
			while (true)
			{
				const size_t index = counter.fetch_add(1);

				if (index >= container.Size())
					break;

				function(container[index]);
			}
		}));
	}

	for (auto& questLog : questLogs)
	{
		questLog.WaitForCompletion();
	}
}