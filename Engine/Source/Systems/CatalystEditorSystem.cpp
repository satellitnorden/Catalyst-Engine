#if defined(CATALYST_EDITOR)
//Header file.
#include <Systems/CatalystEditorSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>

//Third party.
#include <ThirdParty/imgui.h>
#include <ThirdParty/imgui_impl_win32.h>

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

	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(CatalystPlatform::_Window);

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<CatalystEditorSystem *const RESTRICT>(arguments)->PreUpdate();
	},
	this,
	UpdatePhase::PRE,
	UpdatePhase::ENTITY,
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
	ImGui_ImplWin32_Shutdown();
}

/*
*	Updates the Catalyst editor system during the PRE update phase.
*/
void CatalystEditorSystem::PreUpdate() NOEXCEPT
{
	//Begin the new ImGui frame.
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (_IsInGame)
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
	
	else
	{
		//Add the main window.
		ImGui::Begin("Catalyst Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(8.0f, 8.0f));
		ImGui::SetWindowSize(ImVec2(256.0f, 64.0f));

		if (ImGui::Button("Enter Game"))
		{
			_IsInGame = true;
		}

		ImGui::End();
	}
}
#endif