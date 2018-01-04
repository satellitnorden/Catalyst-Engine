#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <Quest.h>

class QuestLog
{

public:

	/*
	*	Default constructor.
	*/
	QuestLog() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~QuestLog() CATALYST_NOEXCEPT;

	/*
	*	Constructor taking all values as arguments.
	*/
	QuestLog(Quest *CATALYST_RESTRICT newQuest) CATALYST_NOEXCEPT;

	/*
	*	Waits for the completion of the quest.
	*/
	void WaitForCompletion() const CATALYST_NOEXCEPT;

private:

	//The underlying quest handle.
	Quest *CATALYST_RESTRICT quest{ nullptr };

};