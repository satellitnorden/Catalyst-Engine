#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor level system.
*/
void EditorLevelSystem::Update() NOEXCEPT
{
	//Is the current contextual window LEVEL?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::LEVEL)
	{
		return;
	}

	//Add the level window.
	ImGui::Begin("Level", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(8.0f, 8.0f + 256.0f + 8.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the button for creating a new level.
	if (ImGui::Button("New Level"))
	{
		DynamicString chosen_file;

		if (File::BrowseForFile(false, &chosen_file))
		{
			int x = 0;
		}
	}

	//Add the button for opening a level.
	if (ImGui::Button("Open Level"))
	{

	}

	//Add the button for saving the current level.
	if (ImGui::Button("Save Level"))
	{

	}

	ImGui::End();
}
#endif