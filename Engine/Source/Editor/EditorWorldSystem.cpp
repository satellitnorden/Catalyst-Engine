#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorWorldSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor world system.
*/
void EditorWorldSystem::Update() NOEXCEPT
{
	//Is the current contextual window WORLD?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::WORLD)
	{
		return;
	}

	//Begin the window.
	ImGui::Begin("World", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the time of day slider.
	{
		float32 current_time_of_day{ WorldSystem::Instance->GetTimeOfDaySystem()->GetCurrentTimeOfDay() };

		if (ImGui::DragFloat("Time Of Day", &current_time_of_day, 0.25f))
		{
			WorldSystem::Instance->GetTimeOfDaySystem()->SetCurrentTimeOfDay(current_time_of_day);
		}
	}

	//End the window.
	ImGui::End();
}
#endif