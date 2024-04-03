#if defined(CATALYST_EDITOR)
//Header file.
#include <Systems/CatalystEditorSystem.h>

//Editor.
#include <Editor/EditorCore.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/ContentSystem.h>
#include <Systems/ImGuiSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

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
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<CatalystEditorSystem *const RESTRICT>(arguments)->GameplayUpdate();
		},
		this,
		UpdatePhase::GAMEPLAY,
		UpdatePhase::USER_INTERFACE,
		true,
		false
	);
}

/*
*	Post initializes the Catalyst editor system.
*/
void CatalystEditorSystem::PostInitialize() NOEXCEPT
{
	//Create all editor icons.
	_EditorIcons[UNDERLYING(EditorIcon::PLAY)] = RenderingSystem::Instance->CreateImGuiTexture(ContentSystem::Instance->GetAsset<Texture2DAsset>(HashString("Editor_Play_Icon"))->_Texture2DHandle);
	_EditorIcons[UNDERLYING(EditorIcon::CREATE)] = RenderingSystem::Instance->CreateImGuiTexture(ContentSystem::Instance->GetAsset<Texture2DAsset>(HashString("Editor_Create_Icon"))->_Texture2DHandle);
	_EditorIcons[UNDERLYING(EditorIcon::TRANSLATE)] = RenderingSystem::Instance->CreateImGuiTexture(ContentSystem::Instance->GetAsset<Texture2DAsset>(HashString("Editor_Translate_Icon"))->_Texture2DHandle);

	//Create the viewport texture ID.
	_ViewportTextureID = RenderingSystem::Instance->CreateImGuiTexture(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::EDITOR_VIEWPORT));

	/*
	//Register for the viewport editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::VIEWPORT,
		[](const Vector2<float32> minimum, const Vector2<float32> maximum)
		{
			return CatalystEditorSystem::Instance->ViewportUpdate(minimum, maximum);
		}
	);
	*/

	//Register for the top bar editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::TOP_BAR,
		[](const Vector2<float32> minimum, const Vector2<float32> maximum)
		{
			return CatalystEditorSystem::Instance->TopBarUpdate(minimum, maximum);
		}
	);

	//Post initialize sub-systems.
	_EditorLevelSystem.PostInitialize();

	//Set up default world parameters.
	WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled = false;
	WorldSystem::Instance->GetSkySystem()->SetSkyTexture(ContentSystem::Instance->GetAsset<TextureCubeAsset>(HashString("Editor_Sky")));
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

	//Update the editor camera system.
	_EditorCameraSystem.Update();

	//Update the editor post-processing system.
	_EditorPostProcessingSystem.Update();

	//Update the editor rendering system.
	_EditorRenderingSystem.Update();

	//Update the editor selection system.
	_EditorSelectionSystem.Update();
}

/*
*	The viewport update.
*/
NO_DISCARD bool CatalystEditorSystem::ViewportUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::Instance->BeginWindow("Editor Viewport", minimum, maximum, false, false, false, false);

	//Retrieve the window resolution.
	const Vector2<float32> window_resolution{ static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height) };

	//Calculate the window size.
	const Vector2<float32> window_size{ window_resolution._X * (maximum._X - minimum._X), window_resolution._Y * (maximum._Y - minimum._Y) };

	//Add the image.
	ImGui::Image(_ViewportTextureID, ImVec2(window_size._X, window_size._Y));

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return true;
}

/*
*	The top bar update.
*/
NO_DISCARD bool CatalystEditorSystem::TopBarUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Define constants.
	constexpr float32 IMAGE_SIZE{ 48.0f };

	//Begin the window.
	ImGuiSystem::Instance->BeginWindow("Editor Top Bar", minimum, maximum, false, false, false, true);

	//Main menu.
	{
		//Add the menu.
		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Level"))
			{
				_EditorLevelSystem.SaveLevel();
			}

			if (ImGui::MenuItem("Save Level As..."))
			{
				_EditorLevelSystem.SaveLevelAs();
			}

			if (ImGui::MenuItem("Load Level"))
			{
				_EditorLevelSystem.LoadLevel();
			}

			if (ImGui::MenuItem("Exit"))
			{
				CatalystEngineSystem::Instance->SetShouldTerminate();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Rendering"))
		{
			if (RenderingSystem::Instance->GetRenderingReferenceSystem()->IsRenderingReferenceInProgress())
			{
				if (ImGui::MenuItem("Stop Rendering Reference"))
				{
					//TODO!
				}
			}

			else
			{
				if (ImGui::MenuItem("Start Rendering Reference"))
				{
					RenderingSystem::Instance->GetRenderingReferenceSystem()->StartRenderingReference();
				}
			}

			ImGui::EndMenu();
		}

		//End the menu.
		ImGui::EndMenuBar();
	}

	//Add the "Play" button.
	if (ImGui::ImageButton(_EditorIcons[UNDERLYING(EditorIcon::PLAY)], ImVec2(IMAGE_SIZE, IMAGE_SIZE)))
	{
		SetIsInGame(true);
	}

	//Layout thing horizontally for the top bar.
	ImGui::SameLine();

	//Add the "Create" button.
	if (ImGui::ImageButton(_EditorIcons[UNDERLYING(EditorIcon::CREATE)], ImVec2(IMAGE_SIZE, IMAGE_SIZE)))
	{
		_EditorLevelSystem.CreateEntity();
	}

	/*
	//Layout thing horizontally for the top bar.
	ImGui::SameLine();

	//Add the "Create" button.
	if (ImGui::ImageButton(_EditorIcons[UNDERLYING(EditorIcon::TRANSLATE)], ImVec2(62.0f, 62.0f)))
	{
		
	}
	*/

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return true;
}
#endif