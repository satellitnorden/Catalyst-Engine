//Header file.
#include <Adventurer.h>

//Multithreading.
#include <Quest.h>

//Systems.
#include <QuestSystem.h>

/*
*	Default constructor.
*/
Adventurer::Adventurer() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
Adventurer::~Adventurer() CATALYST_NOEXCEPT
{

}

/*
*	Updates this adventurer.
*/
void Adventurer::Update() CATALYST_NOEXCEPT
{
	//Carry out quests while this adventurer hasn't been told otherwise.
	while (shouldCarryOutQuests)
	{
		//Check if there's a new quest that can be carried out.
		if (Quest * CATALYST_RESTRICT newQuest = QuestSystem::Instance->GetNewQuest())
		{
			//Mark the quest as in progress.
			newQuest->SetCompletionState(QuestCompletionState::InProgress);

			//Carry out the quest!
			newQuest->GetFunction()();

			//Mark the quest as completed.
			newQuest->SetCompletionState(QuestCompletionState::Complete);
		}

		else
			std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}