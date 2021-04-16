#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorPerceiverSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor perceiver systen .
*/
void EditorPerceiverSystem::Update() NOEXCEPT
{
	//Define constants.
	constexpr float32 MOUSE_ROTATION_SPEED{ CatalystBaseMath::DegreesToRadians(90.0f) };
	constexpr float32 GAMEPAD_ROTATION_SPEED{ CatalystBaseMath::DegreesToRadians(45.0f) };
	
	//Is the current contextual window PERCEIVER?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() == CatalystEditorSystem::ContextualWindow::PERCEIVER)
	{
		//Add the entities window.
		ImGui::Begin("Perceiver", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
		ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

		ImGui::DragFloat("Movement Speed", &_MovementSpeed);

		ImGui::End();
	}

	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Cache the input state.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState() };
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState() };
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState() };

	//Cache if keyboard/mouse rotation should be updated.
	const bool update_keyboard_mouse_rotation{ mouse_state->_Right == ButtonState::PRESSED || mouse_state->_Right == ButtonState::PRESSED_HELD };

	//Update the rotation.
#if 1 //Use keyboard/mouse controls.
	if (update_keyboard_mouse_rotation)
	{
		_Rotation._Roll -= mouse_state->_DeltaY * MOUSE_ROTATION_SPEED;
		_Rotation._Yaw += mouse_state->_DeltaX * MOUSE_ROTATION_SPEED;
	}
#endif
#if 1 //Use gamepad controls.
	_Rotation._Roll -= gamepad_state->_RightThumbstickY * GAMEPAD_ROTATION_SPEED * delta_time;
	_Rotation._Yaw += gamepad_state->_RightThumbstickX * GAMEPAD_ROTATION_SPEED * delta_time;
#endif

	//Cache the forward and right vector.
	const Vector3<float32> forward_vector{ CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_Rotation) };
	const Vector3<float32> right_vector{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_Rotation) };

	//Update the position.
#if 1 //Use keyboard/mouse controls.
	_Position -= CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::Q) == ButtonState::PRESSED) * _MovementSpeed * delta_time;
	_Position -= CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::Q) == ButtonState::PRESSED_HELD) * _MovementSpeed * delta_time;
	_Position += CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::E) == ButtonState::PRESSED) * _MovementSpeed * delta_time;
	_Position += CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::E) == ButtonState::PRESSED_HELD) * _MovementSpeed * delta_time;
	_Position -= forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED) * _MovementSpeed * delta_time;
	_Position -= forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED_HELD) * _MovementSpeed * delta_time;
	_Position += forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED) * _MovementSpeed * delta_time;
	_Position += forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED_HELD) * _MovementSpeed * delta_time;
	_Position -= right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED) * _MovementSpeed * delta_time;
	_Position -= right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED_HELD) * _MovementSpeed * delta_time;
	_Position += right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED) * _MovementSpeed * delta_time;
	_Position += right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD) * _MovementSpeed * delta_time;
#endif
#if 1 //Use gamepad controls.
	_Position += forward_vector * gamepad_state->_LeftThumbstickY * _MovementSpeed * delta_time;
	_Position += right_vector * gamepad_state->_LeftThumbstickX * _MovementSpeed * delta_time;
#endif

	//Set the world transform for the Perceiver.
	{
		WorldTransform world_transform{ _Position, _Rotation, 1.0f };
		Perceiver::Instance->SetWorldTransform(world_transform);
	}
}
#endif