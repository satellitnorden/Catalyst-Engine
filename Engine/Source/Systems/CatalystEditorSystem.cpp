#if defined(CATALYST_EDITOR)
//Header file.
#include <Systems/CatalystEditorSystem.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

//Singleton definition.
DEFINE_SINGLETON(CatalystEditorSystem);

/*
*	Initializes the Catalyst editor system.
*/
void CatalystEditorSystem::Initialize() NOEXCEPT
{
	//Set up ImGui.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<CatalystEditorSystem *const RESTRICT>(arguments)->UserInterfaceUpdate();
	},
	this,
	UpdatePhase::USER_INTERFACE,
	UpdatePhase::LOGIC,
	false);

	//Initialize the editor selection system.
	_EditorSelectionSystem.Initialize();
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
*	Updates the Catalyst editor system during the USER_INTERFACE update phase.
*/
void CatalystEditorSystem::UserInterfaceUpdate() NOEXCEPT
{
	//Update IO.
	UpdateIO();

	//Begin the new ImGui frame.
	ImGui::NewFrame();

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
*	Updates IO.
*/
void CatalystEditorSystem::UpdateIO() NOEXCEPT
{
	//Fill in ImGui's IO struct.
	ImGuiIO& io{ ImGui::GetIO() };

	io.DisplaySize.x = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width);
	io.DisplaySize.y = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height);
	io.DeltaTime = CatalystBaseMath::Maximum<float32>(CatalystEngineSystem::Instance->GetDeltaTime(), FLOAT32_EPSILON);
	io.MousePos = ImVec2(InputSystem::Instance->GetMouseState()->_CurrentX * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), (1.0f - InputSystem::Instance->GetMouseState()->_CurrentY) * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height));
	io.MouseDown[0] = InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED_HELD;
	io.MouseDown[1] = InputSystem::Instance->GetMouseState()->_Right == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_Right == ButtonState::PRESSED_HELD;
	io.MouseDown[2] = InputSystem::Instance->GetMouseState()->_ScrollWheel == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_ScrollWheel == ButtonState::PRESSED_HELD;
	io.MouseWheel = static_cast<float32>(InputSystem::Instance->GetMouseState()->_ScrollWheelStep);
	io.KeyCtrl = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightControl) == ButtonState::PRESSED;
	io.KeyShift = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED;
	io.KeyAlt = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftAlt) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightAlt) == ButtonState::PRESSED;
	io.KeySuper = InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftWindows) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::RightWindows) == ButtonState::PRESSED;

	//Update the KeyMap.
	io.KeyMap[ImGuiKey_Tab] = UNDERLYING(KeyboardButton::Tab);
	io.KeyMap[ImGuiKey_LeftArrow] = UNDERLYING(KeyboardButton::LeftArrow);
	io.KeyMap[ImGuiKey_RightArrow] = UNDERLYING(KeyboardButton::RightArrow);
	io.KeyMap[ImGuiKey_UpArrow] = UNDERLYING(KeyboardButton::UpArrow);
	io.KeyMap[ImGuiKey_DownArrow] = UNDERLYING(KeyboardButton::DownArrow);
	io.KeyMap[ImGuiKey_PageUp] = UNDERLYING(KeyboardButton::PageUp);
	io.KeyMap[ImGuiKey_PageDown] = UNDERLYING(KeyboardButton::PageDown);
	io.KeyMap[ImGuiKey_Home] = UNDERLYING(KeyboardButton::Home);
	io.KeyMap[ImGuiKey_End] = UNDERLYING(KeyboardButton::Home);
	io.KeyMap[ImGuiKey_Insert] = UNDERLYING(KeyboardButton::Insert);
	io.KeyMap[ImGuiKey_Delete] = UNDERLYING(KeyboardButton::Delete);
	io.KeyMap[ImGuiKey_Backspace] = UNDERLYING(KeyboardButton::Backspace);
	io.KeyMap[ImGuiKey_Space] = UNDERLYING(KeyboardButton::Spacebar);
	io.KeyMap[ImGuiKey_Enter] = UNDERLYING(KeyboardButton::Enter);
	io.KeyMap[ImGuiKey_Escape] = UNDERLYING(KeyboardButton::Escape);
	io.KeyMap[ImGuiKey_KeyPadEnter] = UNDERLYING(KeyboardButton::Enter);
	io.KeyMap[ImGuiKey_A] = UNDERLYING(KeyboardButton::A);
	io.KeyMap[ImGuiKey_C] = UNDERLYING(KeyboardButton::C);
	io.KeyMap[ImGuiKey_V] = UNDERLYING(KeyboardButton::V);
	io.KeyMap[ImGuiKey_X] = UNDERLYING(KeyboardButton::X);
	io.KeyMap[ImGuiKey_Y] = UNDERLYING(KeyboardButton::Y);
	io.KeyMap[ImGuiKey_Z] = UNDERLYING(KeyboardButton::Z);

	//Update the KeyDown array.
	for (uint8 i{ 0 }; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
	{
		io.KeysDown[i] = InputSystem::Instance->GetKeyboardState()->GetButtonState(static_cast<KeyboardButton>(i)) == ButtonState::PRESSED;
	}
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
	ImGui::SetWindowPos(ImVec2(8.0f, 8.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 64.0f));

	ImGui::Text("Press ESCAPE to exit game");

	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::Escape) == ButtonState::PRESSED)
	{
		_IsInGame = false;
	}

	ImGui::End();
}

/*
*	Updates when the Catalyst editor is not in a game.
*/
void CatalystEditorSystem::UpdateNotInGame() NOEXCEPT
{
	//Add the main window.
	AddMainWindow();

	//Add the contextual window, if there is any.
	if (_CurrentContextualWindow != ContextualWindow::NONE)
	{
		AddContextualWindow();
	}

	//Update the editor entitiy system.
	_EditorEntitySystem.Update();

	//Update the editor Perceiver system.
	_EditorPerceiverSystem.Update();

	//Update the editor selection system.
	_EditorSelectionSystem.Update();
}

/*
*	Adds the main window.
*/
void CatalystEditorSystem::AddMainWindow() NOEXCEPT
{
	//Add the main window.
	ImGui::Begin("Catalyst Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(8.0f, 8.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 128.0f));

	//Add the enter game button.
	if (ImGui::Button("Enter Game"))
	{
		_IsInGame = true;
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

	//Opens the perceiver window.
	if (_CurrentContextualWindow == ContextualWindow::PERCEIVER)
	{
		if (ImGui::Button("Perceiver"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Perceiver"))
		{
			_CurrentContextualWindow = ContextualWindow::PERCEIVER;
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

	ImGui::End();
}

/*
*	Adds the contextual window.
*/
void CatalystEditorSystem::AddContextualWindow() NOEXCEPT
{
	switch (_CurrentContextualWindow)
	{
		case ContextualWindow::ENTITIES:
		{
			//Handled by the editor entity system. (:

			break;
		}

		case ContextualWindow::PERCEIVER:
		{
			//Handled by the editor perceiver system. (:

			break;
		}

		case ContextualWindow::RENDERING:
		{
			//Add the rendering window.
			ImGui::Begin("Rendering", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(8.0f, 8.0f + 128.0f + 8.0f));
			ImGui::SetWindowSize(ImVec2(256.0f, 256.0f));

			//Add the start/stop taking screenshot button.
			if (RenderingSystem::Instance->IsTakingScreenshot())
			{
				if (ImGui::Button("Stop Taking Screenshot"))
				{
					RenderingSystem::Instance->StopTakingScreenshot("Catalyst Editor Screenshot.tga");
				}
			}

			else
			{
				if (ImGui::Button("Start Taking Screenshot"))
				{
					RenderingSystem::Instance->StartTakingScreenshot();
				}
			}

			//Add button for toggling indirect lighting quality.
			switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality())
			{
				case RenderingConfiguration::IndirectLightingQuality::LOW:
				{
					if (ImGui::Button("Indirect Lighting Quality: Low"))
					{
						RenderingSystem::Instance->GetRenderingConfiguration()->SetIndirectLightingQuality(RenderingConfiguration::IndirectLightingQuality::HIGH);
					}

					break;
				}

				case RenderingConfiguration::IndirectLightingQuality::HIGH:
				{
					if (ImGui::Button("Indirect Lighting Quality: High"))
					{
						RenderingSystem::Instance->GetRenderingConfiguration()->SetIndirectLightingQuality(RenderingConfiguration::IndirectLightingQuality::LOW);
					}

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Add button to toggle path tracing rendering path.
			static bool path_tracing{ false };
			ImGui::Checkbox("Path Tracing", &path_tracing);

			if (path_tracing)
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetRenderingPath(RenderingConfiguration::RenderingPath::PATH_TRACING);
			}

			else
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetRenderingPath(RenderingConfiguration::RenderingPath::MAIN);
			}

			ImGui::End();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}
#endif