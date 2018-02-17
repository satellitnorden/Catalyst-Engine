#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <DailyGroupQuest.h>
#include <DailyQuest.h>

//Enumeration covering all daily quests.
enum class DailyQuests : uint8
{
	RenderingSystemUpdateDynamicUniformData,
	RenderingSystemUpdatePhysicalEntitiesGraphicsBuffers,
	RenderingSystemUpdateViewFrustumCulling,
	InputSystemAsynchronousUpdate,
	NumberOfDailyQuests
};

class QuestSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(QuestSystem);

	/*
	*	Default constructor.
	*/
	QuestSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~QuestSystem() NOEXCEPT;

	/*
	*	Initializes the quest system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Releases the quest system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the number of adventurers.
	*/
	uint8 GetNumberOfAdventurers() const NOEXCEPT { return numberOfAdventurers; }

	/*
	*	Registers a daily quest.
	*/
	void RegisterDailyQuest(const DailyQuests dailyQuest, DailyQuestFunction newFunction) NOEXCEPT;

	/*
	*	Carries out a daily quest.
	*/
	void CarryOutDailyQuest(const DailyQuests dailyQuest, void *RESTRICT arguments) NOEXCEPT;

	/*
	*	Waits for a daily quest to be completed.
	*/
	void WaitForDailyQuest(const DailyQuests dailyQuest) const NOEXCEPT;

private:

	//Denotes the number of adventurers.
	uint8 numberOfAdventurers;

	//Container for all adventurer threads.
	DynamicArray<std::thread> adventurerThreads;

	//Container for all daily quests.
	StaticArray<DailyQuest, static_cast<uint8>(DailyQuests::NumberOfDailyQuests)> dailyQuests;

	/*
	*	Executes an adventurer.
	*/
	void ExecuteAdventurer() NOEXCEPT;

};