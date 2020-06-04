#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ResourceSystem.h>

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
		NewLevel();
	}

	//Add the button for opening a level.
	if (ImGui::Button("Open Level"))
	{
		OpenLevel();
	}

	//Add the button for saving the current level.
	if (ImGui::Button("Save Level"))
	{
		SaveLevel();
	}

	ImGui::End();
}

/*
*	Called then the "New Level" button is pressed.
*/
void EditorLevelSystem::NewLevel() NOEXCEPT
{
	//Have the user pick a file.
	DynamicString chosen_file;

	if (!File::BrowseForFile(false, &chosen_file))
	{
		return;
	}

	//Retrieve the identifier.
	DynamicString identifier;

	for (int64 i{ static_cast<int64>(chosen_file.Length()) - 1 }; i >= 0; --i)
	{
		if (chosen_file[i] == '\\')
		{
			identifier = &chosen_file[i + 1];

			break;
		}
	}

	//Create the empty level resource.
	{
		//Build the level.
		LevelBuildParameters parameters;

		parameters._OutputFilePath = chosen_file.Data();
		parameters._Identifier = identifier.Data();

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildLevel(parameters);
	}

	//The resource building system automatically adds the ".cr" extension to Catalyst resources, so add it to the chosen file path.
	chosen_file += ".cr";

	//Now load the resource into memory!
	ResourceSystem::Instance->LoadResource(chosen_file.Data());
}

/*
*	Called then the "Open Level" button is pressed.
*/
void EditorLevelSystem::OpenLevel() NOEXCEPT
{

}

/*
*	Called then the "Save Level" button is pressed.
*/
void EditorLevelSystem::SaveLevel() NOEXCEPT
{

}
#endif