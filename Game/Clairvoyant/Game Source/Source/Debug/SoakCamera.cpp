//Header file.
#include <Debug/SoakCamera.h>

//Systems.
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>

/*
*	Initializes the soak camera.
*/
void SoakCamera::Initialize() NOEXCEPT
{
	//Set the player to be the active camera/listener.
	RenderingSystem::Instance->SetActiveCamera(this);
	SoundSystem::Instance->SetActiveListener(this);

	//Randomize a direction.
	const Vector3 rotationVector{ 0.0f, CatalystMath::RandomFloatInRange(0.0f, 360.0f), 0.0f };
	Rotate(rotationVector);
}

/*
*	Updates the soak camera.
*/
void SoakCamera::Update(const float deltaTime) NOEXCEPT
{
	//Continously move the camera.
	Move(GetForwardVector() * 10.0f * deltaTime);

	//Set the Y position a bit above the terrain.
	const Vector3 &currentPosition{ GetPosition() };
	const float currentTerrainHeight{ PhysicsSystem::Instance->GetTerrainHeightAtPosition(currentPosition) };

	Move(Vector3(0.0f, currentTerrainHeight - currentPosition.Y + 10.0f, 0.0f));

	//Update the rotation timer.
	rotationTimer -= deltaTime;

	if (rotationTimer <= 0.0f)
	{
		static float multiplier{ 1.0f };

		rotationAmount = CatalystMath::RandomFloatInRange(1.0f, 5.0f) * multiplier;

		multiplier *= -1.0f;

		rotationTimer += 60.0f;
	}

	//Continously rotate the camera.
	Rotate(Vector3(0.0f, rotationAmount * deltaTime, 0.0f));
}