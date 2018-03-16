#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Enumeration covering all daily group quest completion states.
enum class DailyGroupQuestCompletionState : uint8
{
	Unavailable,
	Available,
	InProgress,
	Complete
};

//Type aliases.
using DailyGroupQuestFunction = void(*)(void *RESTRICT, void *RESTRICT);

class DailyGroupQuest final
{

public:

	/*
	*	Default constructor.
	*/
	DailyGroupQuest() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~DailyGroupQuest() NOEXCEPT;

	/*
	*	Returns the quest completion state for this daily group quest.
	*/
	DailyGroupQuestCompletionState GetQuestCompletionState() const NOEXCEPT { return questCompletionState.load(); }

	/*
	*	Sets the quest completion state for this daily group quest.
	*/
	void SetQuestCompletionState(const DailyGroupQuestCompletionState newQuestCompletionState) NOEXCEPT { questCompletionState.store(newQuestCompletionState); }

	/*
	*	Returns the function to execute for this daily group quest.
	*/
	RESTRICTED DailyGroupQuestFunction GetFunction() NOEXCEPT { return function; }

	/*
	*	Sets the function to execute for this daily group quest.
	*/
	void SetFunction(DailyGroupQuestFunction newFunction) NOEXCEPT { function = newFunction; }

	/*
	*	Returns the arguments to send to the function to execute for this daily quest.
	*/
	RESTRICTED void* GetArguments() NOEXCEPT { return arguments; }

	/*
	*	Sets the arguments to send to the function to execute for this daily quest.
	*/
	void SetArguments(void *RESTRICT newArguments) NOEXCEPT { arguments = newArguments; }

	/*
	*	Returns the container for this daily group quest.
	*/
	RESTRICTED void* GetContainer() NOEXCEPT { return container; }

	/*
	*	Sets the container for this daily group quest.
	*/
	void SetContainer(void *RESTRICT newContainer) NOEXCEPT { container = newContainer; }

	/*
	*	Returns the size of the objects contained in the container in bytes.
	*/
	uint64 GetObjectSize() const NOEXCEPT { return objectSize; }

	/*
	*	Sets the size of the objects contained in the container in bytes.
	*/
	void SetObjectSize(const uint64 newObjectSize) NOEXCEPT { objectSize = newObjectSize; }

	/*
	*	Returns the size of the container.
	*/
	uint64 GetContainerSize() const NOEXCEPT { return containerSize; }

	/*
	*	Sets the size of the container.
	*/
	void SetContainerSize(const uint64 newContainerSize) NOEXCEPT { containerSize = newContainerSize; }

	/*
	*	Returns the current container index.
	*/
	uint64 GetCurrentContainerIndex() const NOEXCEPT { return currentContainerIndex; }

	/*
	*	Sets the current container index.
	*/
	void SetCurrentContainerIndex(const uint64 newCurrentContainerIndex) NOEXCEPT { currentContainerIndex = newCurrentContainerIndex; }

	/*
	*	Returns the number of completions required for this daily group quest to be deemed complete.
	*/
	uint8 GetNumberOfCompletionsRequired() const NOEXCEPT { return numberOfCompletionsRequired; }

	/*
	*	Sets the number of completions required for this daily group quest to be deemed complete.
	*/
	void SetNumberOfCompletionsRequired(const uint8 newNumberOfCompletionsRequired) NOEXCEPT { numberOfCompletionsRequired = newNumberOfCompletionsRequired; }

	/*
	*	Returns the current number of completions.
	*/
	uint8 GetCurrentNumberOfCompletions() const NOEXCEPT { return currentNumberOfCompletions.load(); }

	/*
	*	Sets the current number of completions.
	*/
	void SetCurrentNumberOfCompletions(const uint8 newCurrentNumberOfCompletions) NOEXCEPT { currentNumberOfCompletions.store(newCurrentNumberOfCompletions); }

	/*
	*	Carries out this daily quest. Returns whether or not the quest was actually carried out.
	*/
	bool CarryOut() NOEXCEPT;

private:

	//The quest completion state.
	std::atomic<DailyGroupQuestCompletionState> questCompletionState{ DailyGroupQuestCompletionState::Unavailable };

	//The function to execute for this daily quest.
	DailyGroupQuestFunction function;

	//The arguments to send to the function to execute for this daily quest.
	void *RESTRICT arguments;

	//Pointer to the container.
	void *RESTRICT container;

	//The size of the objects contained in the container in bytes.
	uint64 objectSize;

	//The size of the container
	uint64 containerSize;

	//Keeps track of the current container index.
	std::atomic<uint64> currentContainerIndex;

	//The number of completions required for this daily group quest to be deemed complete.
	uint8 numberOfCompletionsRequired;

	//The current number of completions.
	std::atomic<uint8> currentNumberOfCompletions;

};