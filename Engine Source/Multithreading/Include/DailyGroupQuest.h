#pragma once

//Engine core.
#include <EngineCore.h>

//Enumeration covering all daily group quest completion states.
enum class DailyGroupQuestCompletionState : uint8
{
	Unavailable,
	Available,
	InProgress,
	Complete
};

//Type aliases.
using DailyGroupQuestFunction = void(*)(void *CATALYST_RESTRICT);

class DailyGroupQuest
{

public:

	/*
	*	Default constructor.
	*/
	DailyGroupQuest() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~DailyGroupQuest() CATALYST_NOEXCEPT;

	/*
	*	Returns the quest completion state for this daily group quest.
	*/
	DailyGroupQuestCompletionState GetQuestCompletionState() const CATALYST_NOEXCEPT { return questCompletionState.load(); }

	/*
	*	Sets the quest completion state for this daily group quest.
	*/
	void SetQuestCompletionState(const DailyGroupQuestCompletionState newQuestCompletionState) CATALYST_NOEXCEPT { questCompletionState.store(newQuestCompletionState); }

	/*
	*	Returns the container for this daily group quest.
	*/
	CATALYST_RESTRICTED void* GetContainer() CATALYST_NOEXCEPT { return container; }

	/*
	*	Sets the container for this daily group quest.
	*/
	void SetContainer(void *CATALYST_RESTRICT newContainer) CATALYST_NOEXCEPT { container = newContainer; }

	/*
	*	Returns the size of the objects contained in the container in bytes.
	*/
	size_t GetObjectSize() const CATALYST_NOEXCEPT { return objectSize; }

	/*
	*	Sets the size of the objects contained in the container in bytes.
	*/
	void SetObjectSize(const size_t newObjectSize) CATALYST_NOEXCEPT { objectSize = newObjectSize; }

	/*
	*	Returns the function to execute for this daily group quest.
	*/
	CATALYST_RESTRICTED DailyGroupQuestFunction GetFunction() CATALYST_NOEXCEPT { return function; }

	/*
	*	Sets the function to execute for this daily group quest.
	*/
	void SetFunction(DailyGroupQuestFunction newFunction) CATALYST_NOEXCEPT { function = newFunction; }

	/*
	*	Returns the size of the container.
	*/
	size_t GetContainerSize() const CATALYST_NOEXCEPT { return containerSize; }

	/*
	*	Sets the size of the container.
	*/
	void SetContainerSize(const size_t newContainerSize) CATALYST_NOEXCEPT { containerSize = newContainerSize; }

	/*
	*	Returns the current container index.
	*/
	size_t GetCurrentContainerIndex() const CATALYST_NOEXCEPT { return currentContainerIndex; }

	/*
	*	Sets the current container index.
	*/
	void SetCurrentContainerIndex(const size_t newCurrentContainerIndex) CATALYST_NOEXCEPT { currentContainerIndex = newCurrentContainerIndex; }

	/*
	*	Returns the number of completions required for this daily group quest to be deemed complete.
	*/
	uint8 GetNumberOfCompletionsRequired() const CATALYST_NOEXCEPT { return numberOfCompletionsRequired; }

	/*
	*	Sets the number of completions required for this daily group quest to be deemed complete.
	*/
	void SetNumberOfCompletionsRequired(const uint8 newNumberOfCompletionsRequired) CATALYST_NOEXCEPT { numberOfCompletionsRequired = newNumberOfCompletionsRequired; }

	/*
	*	Returns the current number of completions.
	*/
	uint8 GetCurrentNumberOfCompletions() const CATALYST_NOEXCEPT { return currentNumberOfCompletions.load(); }

	/*
	*	Sets the current number of completions.
	*/
	void SetCurrentNumberOfCompletions(const uint8 newCurrentNumberOfCompletions) CATALYST_NOEXCEPT { currentNumberOfCompletions.store(newCurrentNumberOfCompletions); }

	/*
	*	Carries out this daily quest. Returns whether or not the quest was actually carried out.
	*/
	bool CarryOut() CATALYST_NOEXCEPT;

private:

	//The quest completion state.
	std::atomic<DailyGroupQuestCompletionState> questCompletionState{ DailyGroupQuestCompletionState::Unavailable };

	//Pointer to the container.
	void *CATALYST_RESTRICT container;

	//The size of the objects contained in the container in bytes.
	size_t objectSize;

	//The function to execute for this daily quest.
	DailyGroupQuestFunction function;

	//The size of the container
	size_t containerSize;

	//Keeps track of the current container index.
	std::atomic<size_t> currentContainerIndex;

	//The number of completions required for this daily group quest to be deemed complete.
	uint8 numberOfCompletionsRequired;

	//The current number of completions.
	std::atomic<uint8> currentNumberOfCompletions;

};