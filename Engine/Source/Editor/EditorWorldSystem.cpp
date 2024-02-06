#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorWorldSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

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
	ImGui::Begin("World", nullptr, EditorConstants::WINDOW_FLAGS);
	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(0.0f, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

	//Add the sky intensity slider.
	{
		float32 current_sky_intensity{ WorldSystem::Instance->GetSkySystem()->GetSkyIntensity() };

		if (ImGui::DragFloat("Sky Intensity", &current_sky_intensity, 0.1f))
		{
			WorldSystem::Instance->GetSkySystem()->SetSkyIntensity(current_sky_intensity);
		}
	}

	//Add a checkbox to define whether or not volumetric lighting is enabled.
	{
		bool volumetric_lighting_enabled{ WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled };

		if (ImGui::Checkbox("Volumetric Lighting Enabled", &volumetric_lighting_enabled))
		{
			WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled = !WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled;
		}
	}

	//Add volumetric lighting widgets.
	if (WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Enabled)
	{
		{
			float32 current_volumetric_lighting_distance{ WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance };

			if (ImGui::DragFloat("Volumetric Lighting Distance", &current_volumetric_lighting_distance))
			{
				WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance = current_volumetric_lighting_distance;
			}
		}

		{
			float32 current_volumetric_lighting_height{ WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Height };

			if (ImGui::DragFloat("Volumetric Lighting Height", &current_volumetric_lighting_height))
			{
				WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Height = current_volumetric_lighting_height;
			}
		}

		{
			float32 current_volumetric_lighting_thickness{ WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Thickness };

			if (ImGui::DragFloat("Volumetric Lighting Thickness", &current_volumetric_lighting_thickness))
			{
				WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Thickness = current_volumetric_lighting_thickness;
			}
		}
	}

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