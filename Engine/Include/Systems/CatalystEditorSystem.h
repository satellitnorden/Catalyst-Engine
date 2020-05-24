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

	//Denotes if the editor is in a game.
	bool _IsInGame{ false };

	//The editor perceiver system.
	EditorPerceiverSystem _EditorPerceiverSystem;

	//The editor selection system.
	EditorSelectionSystem _EditorSelectionSystem;

	/*
	*	Updates the Catalyst editor system during the PRE update phase.
	*/
	void PreUpdate() NOEXCEPT;

};
#endif