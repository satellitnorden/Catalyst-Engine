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
	GraphicsSystemUpdatePhysicalEntitiesGraphicsBuffers,
	GraphicsSystemUpdateViewFrustumCulling,
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
	*	Carries out a daily quest.
	*/
	void CarryOutDailyQuest(const DailyQuests dailyQuest, void *CATALYST_RESTRICT arguments) CATALYST_NOEXCEPT;

	/*
	*	Waits for a daily quest to be completed.
	*/
	void WaitForDailyQuest(const DailyQuests dailyQuest) const CATALYST_NOEXCEPT;

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
	void ExecuteAdventurer() CATALYST_NOEXCEPT;

};