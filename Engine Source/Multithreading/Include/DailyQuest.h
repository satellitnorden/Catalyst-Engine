#pragma once

//Engine core.
#include <EngineCore.h>

//Enumeration covering all daily quest completion states.
enum class DailyQuestCompletionState : uint8
{
	Unavailable,
	Available,
	InProgress,
	Complete
};

//Type aliases.
using DailyQuestFunction = void(*)(void *CATALYST_RESTRICT);

class DailyQuest
{

public:

	/*
	*	Default constructor.
	*/
	DailyQuest() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~DailyQuest() CATALYST_NOEXCEPT;

	/*
	*	Returns the quest completion state for this daily quest.
	*/
	DailyQuestCompletionState GetQuestCompletionState() const CATALYST_NOEXCEPT { return questCompletionState.load(); }

	/*
	*	Sets the quest completion state for this daily quest.
	*/
	void SetQuestCompletionState(const DailyQuestCompletionState newQuestCompletionState) CATALYST_NOEXCEPT { questCompletionState.store(newQuestCompletionState); }

	/*
	*	Returns the function to execute for this daily quest.
	*/
	CATALYST_RESTRICTED DailyQuestFunction GetFunction() CATALYST_NOEXCEPT { return function; }

	/*
	*	Sets the function to execute for this daily quest.
	*/
	void SetFunction(DailyQuestFunction newFunction) CATALYST_NOEXCEPT { function = newFunction; }

	/*
	*	Returns the arguments for the function to execute for this daily quest.
	*/
	CATALYST_RESTRICTED void* GetArguments() CATALYST_NOEXCEPT { return arguments; }

	/*
	*	Sets the arguments for the function to execute for this daily quest.
	*/
	void SetArguments(void *CATALYST_RESTRICT newArguments) CATALYST_NOEXCEPT { arguments = newArguments; }

	/*
	*	Carries out this daily quest. Returns whether or not the quest was actually carried out.
	*/
	bool CarryOut() CATALYST_NOEXCEPT;

private:

	//The quest completion state.
	std::atomic<DailyQuestCompletionState> questCompletionState{ DailyQuestCompletionState::Unavailable };

	//The function to execute for this daily quest.
	DailyQuestFunction function;

	//The arguments for the function to execute for this daily quest.
	void *CATALYST_RESTRICT arguments;

};