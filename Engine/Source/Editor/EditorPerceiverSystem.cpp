#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorPerceiverSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>

/*
*	Initializes the editor perceiver system.
*/
void EditorPerceiverSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<EditorPerceiverSystem *const RESTRICT>(arguments)->LogicUpdate();
	},
	this,
	UpdatePhase::LOGIC,
	UpdatePhase::PHYSICS,
	false);
}

/*
*	Updates the editor perceiver systen during the LOGIC update phase.
*/
void EditorPerceiverSystem::LogicUpdate() NOEXCEPT
{
	//Define constants.
	constexpr float32 MOUSE_ROTATION_SPEED{ CatalystBaseMath::DegreesToRadians(90.0f) };
	constexpr float32 GAMEPAD_ROTATION_SPEED{ CatalystBaseMath::DegreesToRadians(45.0f) };
	constexpr float32 MOVEMENT_SPEED{ 2.0f };

#if defined(CATALYST_EDITOR)
	//Only update if the editor is not in a game.
	if (CatalystEditorSystem::Instance->IsInGame())
	{
		return;
	}
#endif

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
		_Rotation._X -= mouse_state->_DeltaY * MOUSE_ROTATION_SPEED;
		_Rotation._Y += mouse_state->_DeltaX * MOUSE_ROTATION_SPEED;
	}
#endif
#if 1 //Use gamepad controls.
	_Rotation._X -= gamepad_state->_RightThumbstickY * GAMEPAD_ROTATION_SPEED * delta_time;
	_Rotation._Y += gamepad_state->_RightThumbstickX * GAMEPAD_ROTATION_SPEED * delta_time;
#endif

	//Cache the forward and right vector.
	const Vector3<float32> forward_vector{ CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_Rotation) };
	const Vector3<float32> right_vector{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_Rotation) };

	//Update the position.
#if 1 //Use keyboard/mouse controls.
	_Position -= CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::Q) == ButtonState::PRESSED) * MOVEMENT_SPEED * delta_time;
	_Position -= CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::Q) == ButtonState::PRESSED_HELD) * MOVEMENT_SPEED * delta_time;
	_Position += CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::E) == ButtonState::PRESSED) * MOVEMENT_SPEED * delta_time;
	_Position += CatalystWorldCoordinateSpace::UP * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::E) == ButtonState::PRESSED_HELD) * MOVEMENT_SPEED * delta_time;
	_Position -= forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED) * MOVEMENT_SPEED * delta_time;
	_Position -= forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED_HELD) * MOVEMENT_SPEED * delta_time;
	_Position += forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED) * MOVEMENT_SPEED * delta_time;
	_Position += forward_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED_HELD) * MOVEMENT_SPEED * delta_time;
	_Position -= right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED) * MOVEMENT_SPEED * delta_time;
	_Position -= right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED_HELD) * MOVEMENT_SPEED * delta_time;
	_Position += right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED) * MOVEMENT_SPEED * delta_time;
	_Position += right_vector * static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD) * MOVEMENT_SPEED * delta_time;
#endif
#if 1 //Use gamepad controls.
	_Position += forward_vector * gamepad_state->_LeftThumbstickY * MOVEMENT_SPEED * delta_time;
	_Position += right_vector * gamepad_state->_LeftThumbstickX * MOVEMENT_SPEED * delta_time;
#endif

	//Set the world transform for the Perceiver.
	{
		WorldTransform world_transform{ _Position, _Rotation, 1.0f };
		Perceiver::Instance->SetWorldTransform(world_transform);
	}
}
#endif