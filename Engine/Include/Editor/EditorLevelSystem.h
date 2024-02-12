#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//World.
#include <World/Level/LevelEntry.h>

class EditorLevelSystem final
{

public:

	/*
	*	Updates the editor level system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Starts the game.
	*/
	void StartGame() NOEXCEPT;

	/*
	*	Ends the game.
	*/
	void EndGame() NOEXCEPT;

	/*
	*	Creates an entity.
	*/
	void CreateEntity() NOEXCEPT;

private:

	//The name counter.
	uint32 _NameCounter{ 0 };

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

};

#endif
