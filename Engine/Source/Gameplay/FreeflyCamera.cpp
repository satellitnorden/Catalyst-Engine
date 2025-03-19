//Header file.
#include <Gameplay/FreeflyCamera.h>

//Systems.
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes this freefly camera.
*/
void FreeflyCamera::Initialize() NOEXCEPT
{

}

/*
*	Updates this freefly camera.
*/
void FreeflyCamera::Update(const float32 delta_time) NOEXCEPT
{
	//Cache the world transform.
	WorldTransform world_transform{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform() };

	//Cache the world position.
	WorldPosition world_position{ world_transform.GetWorldPosition() };

	//Update the current input state.
	UpdateCurrentInputState(delta_time);

	//Update the current speed.
	for (int8 i{ 0 }; i < _CurrentInputState._SpeedIncrement; ++i)
	{
		_CurrentSpeed *= 2.0f;
	}

	for (int8 i{ 0 }; i > _CurrentInputState._SpeedIncrement; --i)
	{
		_CurrentSpeed *= 0.5f;
	}

	//Apply rotation.
	_Rotation._Roll += _CurrentInputState._Rotation._X * delta_time * _RotationFactor;
	_Rotation._Yaw += _CurrentInputState._Rotation._Y * delta_time * _RotationFactor;
	_Rotation._Roll = BaseMath::Clamp<float32>(_Rotation._Roll, BaseMath::DegreesToRadians(-89.0f), BaseMath::DegreesToRadians(89.0f));

	//Calculate the forward and right vector.
	Vector3<float32> forward{ CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_Rotation) };
	Vector3<float32> right{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_Rotation) };

	//Apply the movements.
	world_position += forward * _CurrentInputState._Movement._X * delta_time * _CurrentSpeed;
	world_position += CatalystWorldCoordinateSpace::UP * _CurrentInputState._Movement._Y * delta_time * _CurrentSpeed;
	world_position += right * _CurrentInputState._Movement._Z * delta_time * _CurrentSpeed;

	//Apply the current input state.
	world_transform.SetWorldPosition(world_position);
	const Quaternion roll_rotation{ right, _Rotation._Roll };
	const Quaternion yaw_rotation{ CatalystWorldCoordinateSpace::UP, _Rotation._Yaw };
	world_transform.SetRotation(roll_rotation * yaw_rotation);

	RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->SetWorldTransform(world_transform);
}

/*
*	Updates the current input state.
*/
void FreeflyCamera::UpdateCurrentInputState(const float32 delta_time) NOEXCEPT
{
	//Define constants.
	constexpr float32 GAMEPAD_ROTATION_SPEED{ 2.5f };
	constexpr float32 MOUSE_ROTATION_SPEED{ 100.0f };

	//Get the input states.
	const GamepadState* const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState(InputLayer::GAME) };
	const KeyboardState* const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState(InputLayer::GAME) };
	const MouseState* const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::GAME) };

	//Update the speed increment.
	_CurrentInputState._SpeedIncrement = 0;

	_CurrentInputState._SpeedIncrement -= static_cast<int8>(gamepad_state->_LeftShoulder == ButtonState::PRESSED);
	_CurrentInputState._SpeedIncrement += static_cast<int8>(gamepad_state->_RightShoulder == ButtonState::PRESSED);

	_CurrentInputState._SpeedIncrement += mouse_state->_ScrollWheelStep;

	//Update the rotation.
	_CurrentInputState._Rotation._X = 0.0f;
	_CurrentInputState._Rotation._Y = 0.0f;

	_CurrentInputState._Rotation._X += -gamepad_state->_RightThumbstickY * GAMEPAD_ROTATION_SPEED;
	_CurrentInputState._Rotation._Y += gamepad_state->_RightThumbstickX * GAMEPAD_ROTATION_SPEED;

	_CurrentInputState._Rotation._X += -mouse_state->_DeltaY * MOUSE_ROTATION_SPEED;
	_CurrentInputState._Rotation._Y += mouse_state->_DeltaX * MOUSE_ROTATION_SPEED;

	//Update the movement.
	_CurrentInputState._Movement._X = 0.0f;
	_CurrentInputState._Movement._Y = 0.0f;
	_CurrentInputState._Movement._Z = 0.0f;

	_CurrentInputState._Movement._X += gamepad_state->_LeftThumbstickY;
	_CurrentInputState._Movement._Y -= gamepad_state->_CurrentLeftTrigger;
	_CurrentInputState._Movement._Y += gamepad_state->_CurrentRightTrigger;
	_CurrentInputState._Movement._Z += gamepad_state->_LeftThumbstickX;

	_CurrentInputState._Movement._X += static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED_HELD);
	_CurrentInputState._Movement._X -= static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED_HELD);

	_CurrentInputState._Movement._Y -= static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::Q) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::Q) == ButtonState::PRESSED_HELD);
	_CurrentInputState._Movement._Y += static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::E) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::E) == ButtonState::PRESSED_HELD);

	_CurrentInputState._Movement._Z += static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD);
	_CurrentInputState._Movement._Z -= static_cast<float32>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED_HELD);
}