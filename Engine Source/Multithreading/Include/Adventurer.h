#pragma once

//Engine core.
#include <EngineCore.h>

class Adventurer
{

public:

	/*
	*	Default constructor.
	*/
	Adventurer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~Adventurer() CATALYST_NOEXCEPT;

	/*
	*	Updates this adventurer.
	*/
	void Update() CATALYST_NOEXCEPT;

	/*
	*	Tells this adventurer to stop carrying out quests.
	*/
	CATALYST_FORCE_INLINE void StopCarryingOutQuests() CATALYST_NOEXCEPT { shouldCarryOutQuests = false; }

private:

	//Denotes whether or not this adventurer should continue carrying out quests.
	bool shouldCarryOutQuests{ true };

};