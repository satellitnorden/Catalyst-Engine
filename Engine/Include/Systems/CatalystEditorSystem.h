#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Editor.
#include <Editor/EditorEntitySystem.h>
#include <Editor/EditorPerceiverSystem.h>
#include <Editor/EditorSelectionSystem.h>

class CatalystEditorSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEditorSystem);

	//Enumeration covering all contextual windows.
	enum class ContextualWindow : uint8
	{
		NONE,
		ENTITIES,
		RENDERING,
		RESOURCES,
		WORLD
	};

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

	/*
	*	Returns the current contextual window.
	*/
	FORCE_INLINE NO_DISCARD ContextualWindow GetCurrentContextualWindow() const NOEXCEPT
	{
		return _CurrentContextualWindow;
	}

	/*
	*	Returns the editor entity system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorEntitySystem* const RESTRICT GetEditorEntitySystem() NOEXCEPT
	{
		return &_EditorEntitySystem;
	}

private:

	//Denotes if the editor is in a game.
	bool _IsInGame{ false };

	//The current contextual window.
	ContextualWindow _CurrentContextualWindow{ ContextualWindow::NONE };

	//The editor entity system.
	EditorEntitySystem _EditorEntitySystem;

	//The editor perceiver system.
	EditorPerceiverSystem _EditorPerceiverSystem;

	//The editor selection system.
	EditorSelectionSystem _EditorSelectionSystem;

	/*
	*	Updates the Catalyst editor system during the USER_INTERFACE update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

	/*
	*	Updates IO.
	*/
	void UpdateIO() NOEXCEPT;

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