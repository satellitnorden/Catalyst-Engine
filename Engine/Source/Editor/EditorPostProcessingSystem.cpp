#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorPostProcessingSystem.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor post-processing system.
*/
void EditorPostProcessingSystem::Update() NOEXCEPT
{
	//Is the current contextual window RENDERING?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::POST_PROCESSING)
	{
		return;
	}

	//Add the window.
	ImGui::Begin("Post-Processing", nullptr, EditorConstants::WINDOW_FLAGS);
	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(0.0f, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

	//Toggle post-processing enabled.
	ImGui::Checkbox("Post-Processing Enabled", &_PostProcessingEnabled);

	if (_PostProcessingEnabled)
	{
		//Add the tint color editor.
		{
			if (ImGui::ColorEdit3("Tint Color", &_TintColor[0]))
			{
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					_TintColor[i] = CatalystBaseMath::Maximum<float32>(_TintColor[i], 0.0f);
				}
			}
		}

		//Add the tint intensity slider.
		{
			if (ImGui::DragFloat("Tint Intensity", &_TintIntensity, 0.01f))
			{
				_TintIntensity = CatalystBaseMath::Maximum<float32>(_TintIntensity, 0.0f);
			}
		}

		//Add the brightness slider.
		{
			if (ImGui::DragFloat("Brightness", &_Brightness, 0.01f))
			{
				_Brightness = CatalystBaseMath::Maximum<float32>(_Brightness, 0.0f);
			}
		}

		//Add the contrast slider.
		{
			if (ImGui::DragFloat("Contrast", &_Contrast, 0.01f))
			{
				_Contrast = CatalystBaseMath::Maximum<float32>(_Contrast, 0.0f);
			}
		}

		//Add the chromatic aberration intensity slider.
		{
			if (ImGui::DragFloat("Chromatic Aberration Intensity", &_ChromaticAberrationIntensity, 0.001f))
			{
				_ChromaticAberrationIntensity = CatalystBaseMath::Maximum<float32>(_ChromaticAberrationIntensity, 0.0f);
			}
		}

		//Add the exposure slider.
		{
			if (ImGui::DragFloat("Exposure", &_Exposure, 0.01f))
			{
				_Exposure = CatalystBaseMath::Maximum<float32>(_Exposure, 0.0f);
			}
		}

		//Add the saturation slider.
		{
			if (ImGui::DragFloat("Saturation", &_Saturation, 0.01f))
			{
				_Saturation = CatalystBaseMath::Maximum<float32>(_Saturation, 0.0f);
			}
		}

		//Add the depth of field focus distance slider.
		{
			if (ImGui::DragFloat("Depth Of Field Focus Distance", &_DepthOfFieldFocusDistance, 0.01f))
			{
				_DepthOfFieldFocusDistance = CatalystBaseMath::Maximum<float32>(_DepthOfFieldFocusDistance, 0.0f);
			}
		}

		//Add the depth of field size slider.
		{
			if (ImGui::DragFloat("Depth Of Field Size", &_DepthOfFieldSize, 0.01f))
			{
				_DepthOfFieldSize = CatalystBaseMath::Maximum<float32>(_DepthOfFieldSize, 0.0f);
			}
		}

		//Set the values.
		RenderingSystem::Instance->GetPostProcessingSystem()->SetTintColor(_TintColor);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetTintIntensity(_TintIntensity);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetBrightness(_Brightness);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetContrast(_Contrast);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetChromaticAberrationIntensity(_ChromaticAberrationIntensity);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetExposure(_Exposure);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetSaturation(_Saturation);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetDepthOfFieldFocusDistance(_DepthOfFieldFocusDistance);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetDepthOfFieldSize(_DepthOfFieldSize);
	}

	else
	{
		//Set default values.
		RenderingSystem::Instance->GetPostProcessingSystem()->SetTintColor(Vector3<float32>(1.0f, 1.0f, 1.0f));
		RenderingSystem::Instance->GetPostProcessingSystem()->SetTintIntensity(0.0f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetBrightness(1.0f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetContrast(1.0f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetChromaticAberrationIntensity(0.0025f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetExposure(1.0f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetSaturation(1.0f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetDepthOfFieldFocusDistance(1'024.0f);
		RenderingSystem::Instance->GetPostProcessingSystem()->SetDepthOfFieldSize(0.0f);
	}

	//End the window.
	ImGui::End();
}
#endif