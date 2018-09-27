//Header file.
#include <Player/ClairvoyantPlayer.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantPlayer);

/*
*	Initializes the Clairvoyant player.
*/
void ClairvoyantPlayer::Initialize() NOEXCEPT
{
	//Create the camera and set it as the active one..
	_Camera = EntitySystem::Instance->CreateEntity<CameraEntity>();
	RenderingSystem::Instance->SetActiveCamera(_Camera);

	//Register the player for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);
}

/*
*	Updates the Clairvoyant player asynchronously during the logic update phase.
*/
bool ClairvoyantPlayer::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Define constants.
	constexpr float ROTATION_SPEED{ 100.0f };
	constexpr float SPEED{ 100.0f };

	//Get the gamepad state.
	const GamepadState *const RESTRICT state{ InputSystem::Instance->GetGamepadState() };

	//Move the camera.
	_Camera->Move(_Camera->GetRightVector() * state->_LeftThumbstickX * SPEED * context->_DeltaTime);
	_Camera->Move(_Camera->GetForwardVector() * state->_LeftThumbstickY * SPEED * context->_DeltaTime);

	if (state->_LeftShoulder == ButtonState::Pressed
		|| state->_LeftShoulder == ButtonState::PressedHold)
	{
		_Camera->Move(Vector3::DOWN * context->_DeltaTime * SPEED);
	}

	if (state->_RightShoulder == ButtonState::Pressed
		|| state->_RightShoulder == ButtonState::PressedHold)
	{
		_Camera->Move(Vector3::UP * context->_DeltaTime * SPEED);
	}

	//Rotate the camera.
	_Camera->Rotate(Vector3(state->_RightThumbstickY * ROTATION_SPEED * context->_DeltaTime, -state->_RightThumbstickX * ROTATION_SPEED * context->_DeltaTime, 0.0f));

	Vector3 rotation{ _Camera->GetRotation() };
	rotation._X = CatalystBaseMath::Clamp<float>(rotation._X, -89.0f, 89.0f);
	_Camera->SetRotation(rotation);

	return true;
}