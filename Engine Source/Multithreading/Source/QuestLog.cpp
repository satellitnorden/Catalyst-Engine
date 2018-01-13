//Header file.
#include <QuestLog.h>

/*
*	Default destructor.
*/
QuestLog::~QuestLog() CATALYST_NOEXCEPT
{

}

/*
*	Constructor taking all values as arguments.
*/
QuestLog::QuestLog(Quest *CATALYST_RESTRICT newQuest) CATALYST_NOEXCEPT
	:
	quest(newQuest)
{

}

/*
*	Waits for the completion of the quest.
*/
void QuestLog::WaitForCompletion() const CATALYST_NOEXCEPT
{
	while (quest && quest->GetCompletionState() != QuestCompletionState::Complete);
}