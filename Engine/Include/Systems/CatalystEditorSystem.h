#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorCameraSystem.h>
#include <Editor/EditorContentSystem.h>
#include <Editor/EditorContentBrowser.h>
#include <Editor/EditorLevelSystem.h>

//Systems.
#include <Systems/System.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

//Type aliases.
using TopBarMenuCallback = void(*)();

class CatalystEditorSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		CatalystEditorSystem,
		SYSTEM_DEFINED_REQUIREMENT(CATALYST_EDITOR)
		SYSTEM_INITIALIZE()
		SYSTEM_POST_INITIALIZE()
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CatalystEditorSystem() NOEXCEPT
	{

	}

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
	*	Returns the editor content system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorContentSystem *const RESTRICT GetEditorContentSystem() NOEXCEPT
	{
		return &_EditorContentSystem;
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
	*	Registers a top bar menu callback.
	*/
	FORCE_INLINE void RegisterTopBarMenuCallback(const TopBarMenuCallback callback) NOEXCEPT
	{
		_TopBarMenuCallbacks.Emplace(callback);
	}

private:

	//Denotes if the editor is in a game.
	bool _IsInGame{ false };

	//The editor camera system.
	EditorCameraSystem _EditorCameraSystem;

	//The editor content system.
	EditorContentSystem _EditorContentSystem;

	//The editor content browser.
	EditorContentBrowser _EditorContentBrowser;

	//The editor level system.
	EditorLevelSystem _EditorLevelSystem;

	//The editor icons.
	StaticArray<ImTextureID, UNDERLYING(EditorIcon::NUMBER_OF_EDITOR_ICONS)> _EditorIcons;

	//The viewport texture ID.
	ImTextureID _ViewportTextureID;

	//The top bar menu callbacks.
	DynamicArray<TopBarMenuCallback> _TopBarMenuCallbacks;

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
	*	The viewport update.
	*/
	NO_DISCARD bool ViewportUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	The top bar update.
	*/
	NO_DISCARD bool TopBarUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

};
#endif