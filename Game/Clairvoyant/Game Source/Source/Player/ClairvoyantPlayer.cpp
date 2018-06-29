//Header file.
#include <Player/ClairvoyantPlayer.h>

//Components.
#include <Components/ComponentManager.h>

//Engine core.
#include <Engine Core/HashString.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/SpotLightEntity.h>

//Managers.
#include <Managers/PostProcessingManager.h>

//Math.
#include <Math/CatalystMath.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>

//Clairvoyant player constants.
namespace ClairvoyantPlayerConstants
{
	constexpr HashString PARTICLE_MATERIAL{ "ParticleMaterial" };
}

/*
*	Initializes the player.
*/
void ClairvoyantPlayer::Initialize() NOEXCEPT
{
	//Set the player to be the active camera/listener.
	RenderingSystem::Instance->SetActiveCamera(this);
	SoundSystem::Instance->SetActiveListener(this);

	//Add the flashlight.
	flashlight = EntitySystem::Instance->CreateChildEntity<SpotLightEntity>(this);
	flashlight->SetEnabled(false);
	flashlight->SetAttenuationDistance(100.0f);
	flashlight->SetIntensity(100.0f);
	flashlight->Rotate(Vector3(-90.0f, 180.0f, 0.0f));

	//Add a particle system following the player.
	ParticleSystemEntity *const RESTRICT particles{ EntitySystem::Instance->CreateChildEntity<ParticleSystemEntity>(this) };
	particles->Initialize(ResourceLoader::GetParticleMaterial(ClairvoyantPlayerConstants::PARTICLE_MATERIAL), ParticleSystemProperties(5.0f, 60.0f, 0.0f, Vector2(0.05f, 0.05f), Vector2(0.2f, 0.2f), Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f), Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)));
}

/*
*	Updates the player.
*/
void ClairvoyantPlayer::Update(const float deltaTime) NOEXCEPT
{
	static constexpr float cameraLookSpeed{ 50.0f };

	//Get the current gamepad state and use this to control the movement of the character.
	const GamepadState &currentGamepadState{ InputSystem::Instance->GetCurrentGamepadState() };

	if (currentGamepadState.isConnected)
	{
		float movementSpeed{ 2.5f };

		if (constrainToGround)
		{
			movementSpeed += (currentGamepadState.rightTriggerValue * 10.0f);
		}

		else
		{
			movementSpeed += (currentGamepadState.rightTriggerValue * 1'000.0f);
		}

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
		PostProcessingManager::Instance->SetChromaticAbberationStrength(CatalystMath::LinearlyInterpolate(0.0f, 0.001f, currentGamepadState.leftTriggerValue));

		if (currentGamepadState.aButtonState == GamepadButtonState::Pressed)
		{
			constrainToGround = !constrainToGround;
		}
	}

	if (constrainToGround)
	{
		if (ComponentManager::GetNumberOfTerrainComponents() > 0)
		{
			//Always set the position to be 2 meters above the terrain.
			Vector3 currentPosition{ GetPosition() };
			const float terrainHeight{ PhysicsSystem::Instance->GetTerrainHeightAtPosition(currentPosition) };
			const float heightDifference{ terrainHeight - currentPosition.Y };

			Move(Vector3(0.0f, heightDifference + 2.0f, 0.0f));
		}

		else
		{
			Vector3 currentPosition{ GetPosition() };
			const float heightDifference{ 100.0f - currentPosition.Y };

			Move(Vector3(0.0f, heightDifference + 2.0f, 0.0f));
		}
	}
}