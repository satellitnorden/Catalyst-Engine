#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorCameraSystem.h>
#include <Editor/EditorContentBrowser.h>
#include <Editor/EditorLevelSystem.h>
#include <Editor/EditorPostProcessingSystem.h>
#include <Editor/EditorRenderingSystem.h>
#include <Editor/EditorResourcesSystem.h>
#include <Editor/EditorSelectionSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

class ALIGN(8) CatalystEditorSystem final
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
	*	Post initializes the Catalyst editor system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Returns if the editor is in a game.
	*/
	FORCE_INLINE NO_DISCARD bool IsInGame() const NOEXCEPT
	{
		return _IsInGame;
	}

	/*
	*	Returns the editor camera system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorCameraSystem *const RESTRICT GetEditorCameraSystem() NOEXCEPT
	{
		return &_EditorCameraSystem;
	}

	/*
	*	Returns the editor content browser.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorContentBrowser *const RESTRICT GetEditorContentBrowser() NOEXCEPT
	{
		return &_EditorContentBrowser;
	}

	/*
	*	Returns the editor level system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorLevelSystem *const RESTRICT GetEditorLevelSystem() NOEXCEPT
	{
		return &_EditorLevelSystem;
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

	//The editor camera system.
	EditorCameraSystem _EditorCameraSystem;

	//The editor content browser.
	EditorContentBrowser _EditorContentBrowser;

	//The editor level system.
	EditorLevelSystem _EditorLevelSystem;

	//The editor post-processing system.
	EditorPostProcessingSystem _EditorPostProcessingSystem;

	//The editor rendering system.
	EditorRenderingSystem _EditorRenderingSystem;

	//The editor resources system.
	EditorResourcesSystem _EditorResourcesSystem;

	//The editor selection system.
	EditorSelectionSystem _EditorSelectionSystem;

	//The editor icons.
	StaticArray<ImTextureID, UNDERLYING(EditorIcon::NUMBER_OF_EDITOR_ICONS)> _EditorIcons;

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
	*	The top bar update.
	*/
	NO_DISCARD bool TopBarUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

};
#endif