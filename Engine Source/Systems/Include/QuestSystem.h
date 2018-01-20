#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <DailyGroupQuest.h>
#include <DailyQuest.h>

//Enumeration covering all daily quests.
enum class DailyQuests : uint8
{
	GraphicsSystemUpdateDynamicUniformData,
	GraphicsSystemUpdateViewFrustumCulling,
	InputSystemAsynchronousUpdate,
	NumberOfDailyQuests
};

//Enumeration covering all daily quests.
enum class DailyGroupQuests : uint8
{
	GraphicsSystemPhysicalEntityUpdate,
	NumberOfDailyGroupQuests
};

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
	*	Releases the quest system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Returns the number of adventurers.
	*/
	uint8 GetNumberOfAdventurers() const CATALYST_NOEXCEPT { return numberOfAdventurers; }

	/*
	*	Registers a daily quest.
	*/
	void RegisterDailyQuest(const DailyQuests dailyQuest, DailyQuestFunction newFunction) CATALYST_NOEXCEPT;

	/*
	*	Registers a daily group quest.
	*/
	void RegisterDailyGroupQuest(const DailyGroupQuests dailyGroupQuest, DailyGroupQuestFunction function) CATALYST_NOEXCEPT;

	/*
	*	Carries out a daily quest.
	*/
	void CarryOutDailyQuest(const DailyQuests dailyQuest, void *CATALYST_RESTRICT arguments) CATALYST_NOEXCEPT;

	/*
	*	Carries out a daily group quest.
	*/
	void CarryOutDailyGroupQuest(const DailyGroupQuests dailyGroupQuest, void *CATALYST_RESTRICT arguments, void *CATALYST_RESTRICT container, const size_t containerSize, const size_t objectSize) CATALYST_NOEXCEPT;

	/*
	*	Waits for a daily quest to be completed.
	*/
	void WaitForDailyQuest(const DailyQuests dailyQuest) const CATALYST_NOEXCEPT;

	/*
	*	Waits for a daily group quest to be completed.
	*/
	void WaitForDailyGroupQuest(const DailyGroupQuests dailyGroupQuest) const CATALYST_NOEXCEPT;

private:

	//Denotes the number of adventurers.
	uint8 numberOfAdventurers;

	//Container for all adventurer threads.
	DynamicArray<std::thread> adventurerThreads;

	//Container for all daily quests.
	DailyQuest dailyQuests[static_cast<uint8>(DailyQuests::NumberOfDailyQuests)];

	//Container for all daily group quests.
	DailyGroupQuest dailyGroupQuests[static_cast<uint8>(DailyGroupQuests::NumberOfDailyGroupQuests)];

	/*
	*	Executes an adventurer.
	*/
	void ExecuteAdventurer() CATALYST_NOEXCEPT;

};