//Header file.
#include <Quest.h>

/*
*	Constructor taking in it's function as an argument.
*/
Quest::Quest(const std::function<void()> &newFunction) CATALYST_NOEXCEPT
	:
	function(newFunction),
	completionState(QuestCompletionState::Available)
{

}

/*
*	Copy constructor.
*/
Quest::Quest(const Quest &otherQuest) CATALYST_NOEXCEPT
	:
	function(otherQuest.GetFunction()),
	completionState(QuestCompletionState::Available)
{

}