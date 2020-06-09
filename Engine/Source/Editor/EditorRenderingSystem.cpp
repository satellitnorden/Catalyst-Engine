#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorRenderingSystem.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor rendering system.
*/
void EditorRenderingSystem::Update() NOEXCEPT
{
	//Is the current contextual window RENDERING?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::RENDERING)
	{
		return;
	}

	//Add the rendering window.
	ImGui::Begin("Rendering", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

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

	//Add button for toggling ambient occlusion mode
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode())
	{
		case RenderingConfiguration::AmbientOcclusionMode::NONE:
		{
			if (ImGui::Button("Ambient Occlusion Mode: None"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetAmbientOcclusionMode(RenderingConfiguration::AmbientOcclusionMode::SCREEN_SPACE);
			}

			break;
		}

		case RenderingConfiguration::AmbientOcclusionMode::SCREEN_SPACE:
		{
			if (ImGui::Button("Ambient Occlusion Mode: Screen Space"))
			{
				if (RenderingSystem::Instance->IsRayTracingSupported())
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetAmbientOcclusionMode(RenderingConfiguration::AmbientOcclusionMode::RAY_TRACED);
				}

				else
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetAmbientOcclusionMode(RenderingConfiguration::AmbientOcclusionMode::NONE);
				}
			}

			break;
		}

		case RenderingConfiguration::AmbientOcclusionMode::RAY_TRACED:
		{
			if (ImGui::Button("Ambient Occlusion Mode: Ray Traced"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetAmbientOcclusionMode(RenderingConfiguration::AmbientOcclusionMode::NONE);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Add button for toggling indirect lighting mode.
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode())
	{
		case RenderingConfiguration::IndirectLightingMode::NONE:
		{
			if (ImGui::Button("Indirect Lighting Mode: None"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetIndirectLightingMode(RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE);
			}

			break;
		}

		case RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE:
		{
			if (ImGui::Button("Indirect Lighting Mode: Screen Space"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetIndirectLightingMode(RenderingConfiguration::IndirectLightingMode::NONE);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
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

	//Add button for toggling surface shadows mode
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode())
	{
		case RenderingConfiguration::SurfaceShadowsMode::NONE:
		{
			if (ImGui::Button("Surface Shadows Mode: None"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetSurfaceShadowsMode(RenderingConfiguration::SurfaceShadowsMode::RASTERIZED);
			}

			break;
		}

		case RenderingConfiguration::SurfaceShadowsMode::RASTERIZED:
		{
			if (ImGui::Button("Surface Shadows Mode: Rasterized"))
			{
				if (RenderingSystem::Instance->IsRayTracingSupported())
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetSurfaceShadowsMode(RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED);
				}
				
				else
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetSurfaceShadowsMode(RenderingConfiguration::SurfaceShadowsMode::NONE);
				}
			}

			break;
		}

		case RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED:
		{
			if (ImGui::Button("Surface Shadows Mode: Ray Traced"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetSurfaceShadowsMode(RenderingConfiguration::SurfaceShadowsMode::NONE);
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
}
#endif