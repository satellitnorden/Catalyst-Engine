#pragma once

//Engine core.
#include <EngineCore.h>

//Enumeration for all quest completion states.
enum class QuestCompletionState : uint8
{
	Available,
	InProgress,
	Complete
};

class Quest
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	Quest() CATALYST_NOEXCEPT = delete;

	/*
	*	Copy constructor.
	*/
	Quest(const Quest &otherQuest) CATALYST_NOEXCEPT;

	/*
	*	Constructor taking in it's function as an argument.
	*/
	Quest(const std::function<void()> &newFunction) CATALYST_NOEXCEPT;

	/*
	*	Returns a const reference to the function to execute for this quest.
	*/
	CATALYST_FORCE_INLINE const std::function<void()>& GetFunction() const CATALYST_NOEXCEPT { return function; }

	/*
	*	Returns a reference to the function to execute for this quest.
	*/
	CATALYST_FORCE_INLINE std::function<void()>& GetFunction() CATALYST_NOEXCEPT { return function; }

	/*
	*	Returns the completion state of this quest.
	*/
	CATALYST_FORCE_INLINE QuestCompletionState GetCompletionState() const CATALYST_NOEXCEPT { return completionState; }

	/*
	*	Sets the completion state of this quest.
	*/
	CATALYST_FORCE_INLINE void SetCompletionState(const QuestCompletionState newCompletionState) CATALYST_NOEXCEPT { completionState.store(newCompletionState); }

private:

	//The function to execute for this quest.
	std::function<void()> function{ };

	//Defines the completion status of this quest.
	std::atomic<QuestCompletionState> completionState{ QuestCompletionState::Available };

};