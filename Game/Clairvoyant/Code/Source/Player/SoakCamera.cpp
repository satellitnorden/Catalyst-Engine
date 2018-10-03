//Header file.
#include <Player/SoakCamera.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SoakCamera);

/*
*	Initializes the soak camera.
*/
void SoakCamera::Initialize() NOEXCEPT
{
	//Create the camera and set it as the active one.
	_Camera = EntitySystem::Instance->CreateEntity<CameraEntity>();
	RenderingSystem::Instance->SetActiveCamera(_Camera);

	//Register the soak camera for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);

	//Randomize.
	Randomize();


	//Constrain the camera to the ground.
	Vector3 position{ _Camera->GetPosition() };

	float terrainHeight;

	if (TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
	{
		position._Y = terrainHeight + _CurrentHeightOverTerrain;

		_Camera->SetPosition(position);
	}
}

/*
*	Updates the soak camera asynchronously during the logic update phase.
*/
bool SoakCamera::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Define constants.
	constexpr float FLYING_SPEED{ 100.0f };

	//Update the randomize timer.
	_RandomizeTimer += context->_DeltaTime;

	while (_RandomizeTimer >= _RandomizeTime)
	{
		Randomize();
	}

	//Move the camera.
	_Camera->Move(_Camera->GetForwardVector() * FLYING_SPEED * context->_DeltaTime);

	//Constrain the camera to the ground.
	Vector3 position{ _Camera->GetPosition() };

	float terrainHeight;

	if (TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
	{
		if ((terrainHeight + _CurrentHeightOverTerrain) - position._Y >= 0.0f)
		{
			position._Y = CatalystBaseMath::Minimum<float>(position._Y + (FLYING_SPEED * 0.1f * context->_DeltaTime), terrainHeight + _CurrentHeightOverTerrain);
		}
		
		else
		{
			position._Y = CatalystBaseMath::Maximum<float>(position._Y - (FLYING_SPEED * 0.1f * context->_DeltaTime), terrainHeight + _CurrentHeightOverTerrain);
		}

		position._Y = CatalystBaseMath::Maximum<float>(position._Y, CatalystBaseMath::Maximum<float>(terrainHeight + 1.0f, 1.0f));

		_Camera->SetPosition(position);
	}

	//Rotate the camera.
	_Camera->Rotate(Vector3(0.0f, _CurrentRotation * context->_DeltaTime, 0.0f));

	return true;
}

/*
*	Randomizes the properties of the soak camera.
*/
void SoakCamera::Randomize() NOEXCEPT
{
	//Randomize the current height over terrain.
	switch (CatalystBaseMath::RandomIntegerInRange(0, 3))
	{
		case 0:
		{
			_CurrentHeightOverTerrain = 1.0f;

			break;
		}

		case 1:
		{
			_CurrentHeightOverTerrain = 10.0f;

			break;
		}

		case 2:
		{
			_CurrentHeightOverTerrain = 100.0f;

			break;
		}

		case 3:
		{
			_CurrentHeightOverTerrain = 1'000.0f;

			break;
		}
	}

	//Randomize the current rotation.
	_CurrentRotation = CatalystBaseMath::RandomFloatInRange(-2.5f, 2.5f);

	//Randomize the time until the properties of the soak camera is randomized again.
	_RandomizeTime = CatalystBaseMath::RandomFloatInRange(10.0f, 20.0f);

	//Reset the timer.
	_RandomizeTimer = 0.0f;
}