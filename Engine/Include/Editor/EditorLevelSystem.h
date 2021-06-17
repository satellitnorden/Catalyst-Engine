#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class EditorLevelSystem final
{

public:

	/*
	*	Updates the editor level system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Ends the game.
	*/
	void EndGame() NOEXCEPT;

private:

	//Denotes if the user is currently opening a level.
	bool _IsCurrentlyOpeningLevel{ false };

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

};

#endif
