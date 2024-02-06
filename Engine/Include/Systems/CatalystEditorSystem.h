#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Editor.
#include <Editor/EditorCameraSystem.h>
#include <Editor/EditorEntitySystem.h>
#include <Editor/EditorLevelSystem.h>
#include <Editor/EditorPostProcessingSystem.h>
#include <Editor/EditorRenderingSystem.h>
#include <Editor/EditorResourcesSystem.h>
#include <Editor/EditorSelectionSystem.h>
#include <Editor/EditorWorldSystem.h>

class ALIGN(8) CatalystEditorSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEditorSystem);

	//Enumeration covering all contextual windows.
	enum class ContextualWindow : uint8
	{
		NONE,
		CAMERA,
		ENTITIES,
		LEVEL,
		POST_PROCESSING,
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
	*	Returns the editor camera system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorCameraSystem *const RESTRICT GetEditorCameraSystem() NOEXCEPT
	{
		return &_EditorCameraSystem;
	}

	/*
	*	Returns the editor entity system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorEntitySystem *const RESTRICT GetEditorEntitySystem() NOEXCEPT
	{
		return &_EditorEntitySystem;
	}

	/*
	*	Returns the editor post-processing system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorPostProcessingSystem *const RESTRICT GetEditorPostProcessingSystem() NOEXCEPT
	{
		return &_EditorPostProcessingSystem;
	}

	/*
	*	Returns the editor selection system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorSelectionSystem *const RESTRICT GetEditorSelectionSystem() NOEXCEPT
	{
		return &_EditorSelectionSystem;
	}

private:

	//Denotes if the editor is in a game.
	bool _IsInGame{ false };

	//The current contextual window.
	ContextualWindow _CurrentContextualWindow{ ContextualWindow::NONE };

	//The editor camera system.
	EditorCameraSystem _EditorCameraSystem;

	//The editor entity system.
	EditorEntitySystem _EditorEntitySystem;

	//The editor level system.
	EditorLevelSystem _EditorLevelSystem;

	//The editor post-processing system.
	EditorPostProcessingSystem _EditorPostProcessingSystem;

	//The editor rendering system.
	EditorRenderingSystem _EditorRenderingSystem;

	//The editor resources system.
	EditorResourcesSystem _EditorResourcesSystem;

	//The editor world system.
	EditorWorldSystem _EditorWorldSystem;

	//The editor selection system.
	EditorSelectionSystem _EditorSelectionSystem;

	/*
	*	Updates the Catalyst editor system during the GAMEPLAY update phase.
	*/
	void GameplayUpdate() NOEXCEPT;

	/*
	*	Sets whether or not the engine is in game right now.
	*/
	void SetIsInGame(const bool value) NOEXCEPT;

	/*
	*	Starts the game.
	*/
	void StartGame() NOEXCEPT;

	/*
	*	Ends the game.
	*/
	void EndGame() NOEXCEPT;

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

};
#endif