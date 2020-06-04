#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorResourcesSystem.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor resources system.
*/
void EditorResourcesSystem::Update() NOEXCEPT
{
	//Is the current contextual window RESOURCES?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::RESOURCES)
	{
		return;
	}

	//Add the entities window.
	ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(8.0f, 8.0f + 256.0f + 8.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	ImGui::End();
}
#endif