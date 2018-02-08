//Header file.
#include <ClairvoyantPlayer.h>

//Entities.
#include <CameraEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>

//Math.
#include <GameMath.h>

//Systems.
#include <EntitySystem.h>
#include <InputSystem.h>
#include <GraphicsSystem.h>

/*
*	Default constructor.
*/
ClairvoyantPlayer::ClairvoyantPlayer() NOEXCEPT
{

}

/*
*	Default destructor.
*/
ClairvoyantPlayer::~ClairvoyantPlayer() NOEXCEPT
{

}

/*
*	Initializes the player.
*/
void ClairvoyantPlayer::Initialize() NOEXCEPT
{
	//Add a camera.
	GraphicsSystem::Instance->SetActiveCamera(this);

	//Add the flashlight.
	flashlight = EntitySystem::Instance->CreateChildEntity<SpotLightEntity>(this);
	flashlight->SetEnabled(false);
	flashlight->SetAttenuationDistance(100.0f);
	flashlight->SetIntensity(25.0f);
	flashlight->Rotate(Vector3(-90.0f, 180.0f, 0.0f));
	//flashlight->Move(Vector3(0.0f, -1.0f, 0.0f));
}

/*
*	Updates the player.
*/
void ClairvoyantPlayer::Update(const float deltaTime) NOEXCEPT
{
	static const float cameraLookSpeed = 50.0f;

	//Get the current gamepad state and use this to control the movement of the character.
	const GamepadState &currentGamepadState = InputSystem::Instance->GetCurrentGamepadState();

	if (currentGamepadState.isConnected)
	{
		const float movementSpeed = 1.0f + (currentGamepadState.rightTriggerValue * 100.0f);

		//Calculate camera movement.
		Rotate(Vector3(0.0f, currentGamepadState.rightThumbstickXValue * -cameraLookSpeed * deltaTime, 0.0f));
		Rotate(Vector3(currentGamepadState.rightThumbstickYValue * cameraLookSpeed * deltaTime, 0.0f, 0.0f));

		const Vector3 &rotation = GetRotation();

		if (rotation.X > 89.0f)
		{
			const Vector3 rotateVector{ -(rotation.X - 89.0f), 0.0f, 0.0f };
			Rotate(rotateVector);
		}
			
		else if (rotation.X < -89.0f)
		{
			const Vector3 rotateVector{ -(rotation.X + 89.0f), 0.0f, 0.0f };
			Rotate(rotateVector);
		}

		//Move.
		Move(GetRightVector() * currentGamepadState.leftThumbstickXValue * deltaTime * movementSpeed);
		Move(GetForwardVector() * currentGamepadState.leftThumbstickYValue * deltaTime * movementSpeed);

		if (currentGamepadState.rightShoulderButtonState == GamepadButtonState::Pressed || currentGamepadState.rightShoulderButtonState == GamepadButtonState::PressedHold)
		{
			Move(GetUpVector() * deltaTime * movementSpeed);
		}

		else if (currentGamepadState.leftShoulderButtonState == GamepadButtonState::Pressed || currentGamepadState.leftShoulderButtonState == GamepadButtonState::PressedHold)
		{
			Move(GetUpVector() * deltaTime * movementSpeed * -1.0f);
		}

		//Toggle the flashlight.
		if (currentGamepadState.yButtonState == GamepadButtonState::Pressed)
		{
			flashlight->SetEnabled(!flashlight->GetEnabled());
		}

		//Lerp post processing effects.
		GraphicsSystem::Instance->SetPostProcessingChromaticAberrationAmount(GameMath::LinearlyInterpolate(0.0f, 0.01f, currentGamepadState.leftTriggerValue));
	}
}