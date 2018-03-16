#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Enumeration covering all daily quest completion states.
enum class DailyQuestCompletionState : uint8
{
	Unavailable,
	Available,
	InProgress,
	Complete
};

//Type aliases.
using DailyQuestFunction = void(*)(void *RESTRICT);

class DailyQuest final
{

public:

	/*
	*	Default constructor.
	*/
	DailyQuest() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~DailyQuest() NOEXCEPT;

	/*
	*	Returns the quest completion state for this daily quest.
	*/
	DailyQuestCompletionState GetQuestCompletionState() const NOEXCEPT { return questCompletionState.load(); }

	/*
	*	Sets the quest completion state for this daily quest.
	*/
	void SetQuestCompletionState(const DailyQuestCompletionState newQuestCompletionState) NOEXCEPT { questCompletionState.store(newQuestCompletionState); }

	/*
	*	Returns the function to execute for this daily quest.
	*/
	RESTRICTED DailyQuestFunction GetFunction() NOEXCEPT { return function; }

	/*
	*	Sets the function to execute for this daily quest.
	*/
	void SetFunction(DailyQuestFunction newFunction) NOEXCEPT { function = newFunction; }

	/*
	*	Returns the arguments for the function to execute for this daily quest.
	*/
	RESTRICTED void* GetArguments() NOEXCEPT { return arguments; }

	/*
	*	Sets the arguments for the function to execute for this daily quest.
	*/
	void SetArguments(void *RESTRICT newArguments) NOEXCEPT { arguments = newArguments; }

	/*
	*	Carries out this daily quest. Returns whether or not the quest was actually carried out.
	*/
	bool CarryOut() NOEXCEPT;

private:

	//The quest completion state.
	std::atomic<DailyQuestCompletionState> questCompletionState{ DailyQuestCompletionState::Unavailable };

	//The function to execute for this daily quest.
	DailyQuestFunction function;

	//The arguments for the function to execute for this daily quest.
	void *RESTRICT arguments;

};