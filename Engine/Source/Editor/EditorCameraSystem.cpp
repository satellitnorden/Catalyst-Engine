#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorCameraSystem.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

/*
*	Initializes the editor camera system.
*/
void EditorCameraSystem::Initialize() NOEXCEPT
{
	//Initialize the freefly camera.
	_FreeflyCamera.Initialize();
}

/*
*	Updates the editor camera systen.
*/
void EditorCameraSystem::Update() NOEXCEPT
{
	/*
	//Is the current contextual window CAMERA?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() == CatalystEditorSystem::ContextualWindow::CAMERA)
	{
		//Add the entities window.
		ImGui::Begin("Camera", nullptr, EditorConstants::WINDOW_FLAGS);
		EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(0.0f, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

		//Add a slider for the aperture.
		{
			float32 aperture{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetAperture() };

			if (ImGui::DragFloat("Aperture", &aperture, 0.01f, 0.0f, 1.0f))
			{
				RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->SetAperture(aperture);
			}
		}

		//Add a checkbox for automatic focal distance.
		{
			bool automatic_focal_distance{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetAutomaticFocalDistance() };

			if (ImGui::Checkbox("Automatic Focal Distance", &automatic_focal_distance))
			{
				RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->SetAutomaticFocalDistance(automatic_focal_distance);
			}
		}

		//Add a slider for the focal distance.
		{
			float32 focal_distance{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFocalDistance() };

			if (ImGui::DragFloat("Focal Distance", &focal_distance, 0.01f))
			{
				RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->SetFocalDistance(focal_distance);
			}
		}

		ImGui::End();
	}
	*/

	//Cache the input state.
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::DEBUG) };

	//Cache if keyboard/mouse rotation should be updated.
	const bool update_keyboard_mouse_rotation{ mouse_state->_Right == ButtonState::PRESSED || mouse_state->_Right == ButtonState::PRESSED_HELD };

	if (InputSystem::Instance->GetLastUpdatedInputDeviceType() == InputDeviceType::GAMEPAD)
	{
		_FreeflyCamera.SetRotationFactor(1.0f);
	}

	else
	{
		if (update_keyboard_mouse_rotation)
		{
			_FreeflyCamera.SetRotationFactor(1.0f);
			CatalystPlatform::HideCursor();
		}
		
		else
		{
			_FreeflyCamera.SetRotationFactor(0.0f);
			CatalystPlatform::ShowCursor();
		}
	}

	//Update the freefly camera.
	_FreeflyCamera.Update(CatalystEngineSystem::Instance->GetDeltaTime());
}
#endif