#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorRenderingSystem.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

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
	ImGui::Begin("Rendering", nullptr, EditorConstants::WINDOW_FLAGS);
	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(0.0f, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

	//Add the start/stop taking screenshot button.
	if (RenderingSystem::Instance->IsTakingScreenshot())
	{
		if (ImGui::Button("Stop Taking Screenshot"))
		{
			RenderingSystem::Instance->StopTakingScreenshot("Catalyst Editor Screenshot.png");
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
				if (RenderingSystem::Instance->IsRayTracingSupported())
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetIndirectLightingMode(RenderingConfiguration::IndirectLightingMode::RAY_TRACED);
				}

				else
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetIndirectLightingMode(RenderingConfiguration::IndirectLightingMode::NONE);
				}
			}

			break;
		}

		case RenderingConfiguration::IndirectLightingMode::RAY_TRACED:
		{
			if (ImGui::Button("Indirect Lighting Mode: Ray Traced"))
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

	//Add button for toggling surface shadows mode
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetVolumetricShadowsMode())
	{
		case RenderingConfiguration::VolumetricShadowsMode::NONE:
		{
			if (ImGui::Button("Volumetric Shadows Mode: None"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetVolumetricShadowsMode(RenderingConfiguration::VolumetricShadowsMode::SCREEN_SPACE);
			}

			break;
		}

		case RenderingConfiguration::VolumetricShadowsMode::SCREEN_SPACE:
		{
			if (ImGui::Button("Volumetric Shadows Mode: Screen Space"))
			{
				if (RenderingSystem::Instance->IsRayTracingSupported())
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetVolumetricShadowsMode(RenderingConfiguration::VolumetricShadowsMode::RAY_TRACED);
				}

				else
				{
					RenderingSystem::Instance->GetRenderingConfiguration()->SetVolumetricShadowsMode(RenderingConfiguration::VolumetricShadowsMode::NONE);
				}
			}

			break;
		}

		case RenderingConfiguration::VolumetricShadowsMode::RAY_TRACED:
		{
			if (ImGui::Button("Volumetric Shadows Mode: Ray Traced"))
			{
				RenderingSystem::Instance->GetRenderingConfiguration()->SetVolumetricShadowsMode(RenderingConfiguration::VolumetricShadowsMode::NONE);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Add a button to toggle between different rendering paths.
	{
		const RenderingPath current_rendering_path{ RenderingSystem::Instance->GetCurrentRenderingPath() };

		switch (current_rendering_path)
		{
			case RenderingPath::DEFAULT:
			{
				if (ImGui::Button("Rendering Path: Default"))
				{
					RenderingSystem::Instance->SetCurrentRenderingPath(RenderingPath::PATH_TRACING);
				}

				break;
			}

			case RenderingPath::PATH_TRACING:
			{
				if (ImGui::Button("Rendering Path: Path Tracing"))
				{
					RenderingSystem::Instance->SetCurrentRenderingPath(RenderingPath::SIMPLIFIED);
				}

				break;
			}

			case RenderingPath::SIMPLIFIED:
			{
				if (ImGui::Button("Rendering Path: Simplified"))
				{
					RenderingSystem::Instance->SetCurrentRenderingPath(RenderingPath::DEFAULT);
				}

				break;
			}

			case RenderingPath::VIRTUAL_REALITY:
			{
				if (ImGui::Button("Rendering Path: Virtual Reality"))
				{
					RenderingSystem::Instance->SetCurrentRenderingPath(RenderingPath::DEFAULT);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	ImGui::End();
}
#endif