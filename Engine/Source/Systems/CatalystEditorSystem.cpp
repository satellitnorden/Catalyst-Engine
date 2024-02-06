#if defined(CATALYST_EDITOR)
//Header file.
#include <Systems/CatalystEditorSystem.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

//Singleton definition.
DEFINE_SINGLETON(CatalystEditorSystem);

/*
*	Initializes the Catalyst editor system.
*/
void CatalystEditorSystem::Initialize() NOEXCEPT
{
	//Initialize the editor camera system.
	_EditorCameraSystem.Initialize();

	//Initialize the editor selection system.
	_EditorSelectionSystem.Initialize();

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void *const RESTRICT arguments)
	{
		static_cast<CatalystEditorSystem *const RESTRICT>(arguments)->GameplayUpdate();
	},
	this,
	UpdatePhase::GAMEPLAY,
	UpdatePhase::USER_INTERFACE,
	true,
	false);
}

/*
*	Terminates the Catalyst editor system.
*/
void CatalystEditorSystem::Terminate() NOEXCEPT
{
	//Shut down ImGui.
	ImGui::DestroyContext();
}

/*
*	Updates the Catalyst editor system during the GAMEPLAY update phase.
*/
void CatalystEditorSystem::GameplayUpdate() NOEXCEPT
{
	if (_IsInGame)
	{
		UpdateInGame();
	}
	
	else
	{
		UpdateNotInGame();
	}
}

/*
*	Sets whether or not the engine is in game right now.
*/
void CatalystEditorSystem::SetIsInGame(const bool value) NOEXCEPT
{
	if (_IsInGame != value)
	{
		if (_IsInGame)
		{
			EndGame();

			CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._EndGameFunction();
		}

		else
		{
			StartGame();

			CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._StartGameFunction();
		}

		_IsInGame = value;
	}
}

/*
*	Starts the game.
*/
void CatalystEditorSystem::StartGame() NOEXCEPT
{
	_EditorLevelSystem.StartGame();
}

/*
*	Ends the game.
*/
void CatalystEditorSystem::EndGame() NOEXCEPT
{
	_EditorLevelSystem.EndGame();
}

/*
*	Updates when the Catalyst editor is in a game.
*/
void CatalystEditorSystem::UpdateInGame() NOEXCEPT
{
	//Add the game window.
	ImGui::Begin(	"Catalyst Editor",
					nullptr,
					ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoScrollWithMouse
					| ImGuiWindowFlags_NoBackground
					| ImGuiWindowFlags_NoSavedSettings
					| ImGuiWindowFlags_NoMouseInputs
					| ImGuiWindowFlags_NoFocusOnAppearing
					| ImGuiWindowFlags_NoBringToFrontOnFocus
					| ImGuiWindowFlags_NoNav
					| ImGuiWindowFlags_NoDecoration
					| ImGuiWindowFlags_NoInputs);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 64.0f));

	ImGui::Text("Press F11 to exit game");

	if (InputSystem::Instance->GetKeyboardState(InputLayer::DEBUG)->GetButtonState(KeyboardButton::F11) == ButtonState::PRESSED)
	{
		SetIsInGame(false);
	}

	ImGui::End();
}

/*
*	Updates when the Catalyst editor is not in a game.
*/
void CatalystEditorSystem::UpdateNotInGame() NOEXCEPT
{
	//Always show the cursor.
	CatalystPlatform::ShowCursor();

	//Add the main window.
	AddMainWindow();

	//Update the editor camera system.
	_EditorCameraSystem.Update();

	//Update the editor entity system.
	_EditorEntitySystem.Update();

	//Update the editor level system.
	_EditorLevelSystem.Update();

	//Update the editor post-processing system.
	_EditorPostProcessingSystem.Update();

	//Update the editor rendering system.
	_EditorRenderingSystem.Update();

	//Update the editor resources system.
	_EditorResourcesSystem.Update();

	//Update the editor world system.
	_EditorWorldSystem.Update();

	//Update the editor selection system.
	_EditorSelectionSystem.Update();
}

/*
*	Adds the main window.
*/
void CatalystEditorSystem::AddMainWindow() NOEXCEPT
{
	//Add the main window.
	ImGui::Begin("Catalyst Editor", nullptr, EditorConstants::WINDOW_FLAGS);
	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::TOP_LEFT, Vector2<float32>(0.0f, -0.5f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

	//Add the enter game button.
	if (ImGui::Button("Enter Game"))
	{
		SetIsInGame(true);
	}

	//Opens the camera window.
	if (_CurrentContextualWindow == ContextualWindow::CAMERA)
	{
		if (ImGui::Button("Camera"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Camera"))
		{
			_CurrentContextualWindow = ContextualWindow::CAMERA;
		}
	}

	//Opens the entities window.
	if (_CurrentContextualWindow == ContextualWindow::ENTITIES)
	{
		if (ImGui::Button("Entities"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Entities"))
		{
			_CurrentContextualWindow = ContextualWindow::ENTITIES;
		}
	}

	//Opens the level window.
	if (_CurrentContextualWindow == ContextualWindow::LEVEL)
	{
		if (ImGui::Button("Level"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Level"))
		{
			_CurrentContextualWindow = ContextualWindow::LEVEL;
		}
	}

	//Opens the post-processing window.
	if (_CurrentContextualWindow == ContextualWindow::POST_PROCESSING)
	{
		if (ImGui::Button("Post-Processing"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Post-Processing"))
		{
			_CurrentContextualWindow = ContextualWindow::POST_PROCESSING;
		}
	}

	//Opens the rendering window.
	if (_CurrentContextualWindow == ContextualWindow::RENDERING)
	{
		if (ImGui::Button("Rendering"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Rendering"))
		{
			_CurrentContextualWindow = ContextualWindow::RENDERING;
		}
	}

	//Opens the resources window.
	if (_CurrentContextualWindow == ContextualWindow::RESOURCES)
	{
		if (ImGui::Button("Resources"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Resources"))
		{
			_CurrentContextualWindow = ContextualWindow::RESOURCES;
		}
	}

	//Opens the worlds window.
	if (_CurrentContextualWindow == ContextualWindow::WORLD)
	{
		if (ImGui::Button("World"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("World"))
		{
			_CurrentContextualWindow = ContextualWindow::WORLD;
		}
	}

	//Add the "Exit" button.
	if (ImGui::Button("Exit"))
	{
		CatalystEngineSystem::Instance->SetShouldTerminate();
	}

	ImGui::End();
}
#endif