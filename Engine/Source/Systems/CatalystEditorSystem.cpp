#if defined(CATALYST_EDITOR)
//Header file.
#include <Systems/CatalystEditorSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

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
}
#endif