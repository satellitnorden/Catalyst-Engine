#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

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

private:

	//Denotes if the user is currently opening a level.
	bool _IsCurrentlyOpeningLevel{ false };

	//The current level name.
	DynamicString _CurrentLevelName;

	//The current level entries.
	DynamicArray<LevelEntry> _CurrentLevelEntries;

	/*
	*	Called then the "New Level" button is pressed.
	*/
	void NewLevel() NOEXCEPT;

	/*
	*	Called then the "Open Level" button is pressed.
	*/
	void OpenLevel() NOEXCEPT;

	/*
	*	Called then the "Save Level" button is pressed.
	*/
	void SaveLevel() NOEXCEPT;

	/*
	*	Adds the contextual window.
	*/
	void AddContextualWindow();

	/*
	*	Adds the current level window.
	*/
	void AddCurrentLevelWindow() NOEXCEPT;

	/*
	*	Gathers the level entries.
	*/
	void GatherLevelEntries(DynamicArray<LevelEntry> *const RESTRICT output) const NOEXCEPT;

};

#endif
