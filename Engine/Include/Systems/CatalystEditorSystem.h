#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Editor.
#include <Editor/EditorPerceiverSystem.h>
#include <Editor/EditorSelectionSystem.h>

class CatalystEditorSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEditorSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CatalystEditorSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the Catalyst editor system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the Catalyst editor system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns if the editor is in a game.
	*/
	FORCE_INLINE NO_DISCARD bool IsInGame() const NOEXCEPT
	{
		return _IsInGame;
	}

private:

	//Enumeration covering all contextual windows.
	enum class ContextualWindow : uint8
	{
		NONE,
		RENDERING,
		WORLD
	};

	//Denotes if the editor is in a game.
	bool _IsInGame{ false };

	//The current contextual window.
	ContextualWindow _CurrentContextualWindow{ ContextualWindow::NONE };

	//The editor perceiver system.
	EditorPerceiverSystem _EditorPerceiverSystem;

	//The editor selection system.
	EditorSelectionSystem _EditorSelectionSystem;

	/*
	*	Updates the Catalyst editor system during the USER_INTERFACE update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

	/*
	*	Updates when the Catalyst editor is in a game.
	*/
	void UpdateInGame() NOEXCEPT;

	/*
	*	Updates when the Catalyst editor is not in a game.
	*/
	void UpdateNotInGame() NOEXCEPT;

	/*
	*	Adds the main window.
	*/
	void AddMainWindow() NOEXCEPT;

	/*
	*	Adds the contextual window.
	*/
	void AddContextualWindow() NOEXCEPT;

};
#endif