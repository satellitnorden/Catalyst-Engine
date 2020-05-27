#if defined(CATALYST_EDITOR)
//Header file.
#include <Systems/CatalystEditorSystem.h>

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

	//Updates the editor perceiver system.
	_EditorPerceiverSystem.Initialize();

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
	//Fill in ImGui's IO struct.
	ImGuiIO& io{ ImGui::GetIO() };

	io.DisplaySize.x = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width);
	io.DisplaySize.y = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height);
	io.DeltaTime = CatalystBaseMath::Maximum<float32>(CatalystEngineSystem::Instance->GetDeltaTime(), FLOAT32_EPSILON);
	io.MousePos = ImVec2(InputSystem::Instance->GetMouseState()->_CurrentX * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), (1.0f - InputSystem::Instance->GetMouseState()->_CurrentY) * static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height));
	io.MouseDown[0] = InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED_HELD;
	io.MouseDown[1] = InputSystem::Instance->GetMouseState()->_Right == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_Right == ButtonState::PRESSED_HELD;
	io.MouseDown[2] = InputSystem::Instance->GetMouseState()->_ScrollWheel == ButtonState::PRESSED || InputSystem::Instance->GetMouseState()->_ScrollWheel == ButtonState::PRESSED_HELD;

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

	//Opens the world rendering window.
	if (_CurrentContextualWindow == ContextualWindow::RENDERING)
	{
		if (ImGui::Button("Close Rendering"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Open Rendering"))
		{
			_CurrentContextualWindow = ContextualWindow::RENDERING;
		}
	}

	//Opens the world rendering window.
	if (_CurrentContextualWindow == ContextualWindow::RESOURCES)
	{
		if (ImGui::Button("Close Resources"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Open Resources"))
		{
			_CurrentContextualWindow = ContextualWindow::RESOURCES;
		}
	}

	//Opens the world contextual window.
	if (_CurrentContextualWindow == ContextualWindow::WORLD)
	{
		if (ImGui::Button("Close World"))
		{
			_CurrentContextualWindow = ContextualWindow::NONE;
		}
	}

	else
	{
		if (ImGui::Button("Open World"))
		{
			_CurrentContextualWindow = ContextualWindow::WORLD;
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
		case ContextualWindow::RENDERING:
		{
			//Add the rendering window.
			ImGui::Begin("Rendering", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(8.0f, 1'080.0f - 8.0f - 128.0f));
			ImGui::SetWindowSize(ImVec2(1'920.0f - 8.0f - 8.0f, 128.0f));

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

		case ContextualWindow::RESOURCES:
		{
			//Add the world window.
			ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(8.0f, 1'080.0f - 8.0f - 128.0f));
			ImGui::SetWindowSize(ImVec2(1'920.0f - 8.0f - 8.0f, 128.0f));

			//List all models resources, for now.
			const HashTable<HashString, ModelResource* RESTRICT> &all_model_resources{ ResourceSystem::Instance->GetAllModelResources() };

			for (const ModelResource *const RESTRICT model_resource : all_model_resources.ValueIterator())
			{
				ImGui::Text(model_resource->_Header._ResourceName.Data());
			}

			ImGui::End();

			break;
		}

		case ContextualWindow::WORLD:
		{
			//Add the world window.
			ImGui::Begin("World", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(8.0f, 1'080.0f - 8.0f - 128.0f));
			ImGui::SetWindowSize(ImVec2(1'920.0f - 8.0f - 8.0f, 128.0f));

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