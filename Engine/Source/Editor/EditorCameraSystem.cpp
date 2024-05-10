#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorCameraSystem.h>

//Editor.
#include <Editor/EditorCore.h>

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
	//Detect if any text input is active - If so, do nothing.
	if (ImGui::GetIO().WantTextInput)
	{
		return;
	}

	//Only update if no window is hovered.
	//if (ImGui::GetIO().WantCaptureMouse)
	//{
	//	return;
	//}

	//Cache the input state.
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::GAME) };

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