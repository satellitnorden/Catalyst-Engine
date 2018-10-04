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

//Soak camera constants.
namespace SoakCameraConstants
{
	constexpr float OPERATIONAL_SPEED{ 1.0f };
}

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
		position._Y = terrainHeight;

		_Camera->SetPosition(position);
	}
}

/*
*	Updates the soak camera asynchronously during the logic update phase.
*/
bool SoakCamera::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the randomize timer.
	_RandomizeTimer += context->_DeltaTime;

	while (_RandomizeTimer >= _RandomizeTime)
	{
		Randomize();
	}

	//Update the forward movement of the camera.
	UpdateForwardMovement(context);

	//Update the rotation of the camera.
	UpdateRotation(context);

	//Get the current position of the camera.
	Vector3 position{ _Camera->GetPosition() };

	//Look ahead at the terrain height at a position in front of the camera to determine how the velocity should be affected.
	float terrainHeightAhead;
	TerrainSystem::Instance->GetTerrainHeightAtPosition(_Camera->GetPosition() + _Camera->GetForwardVector() * _CurrentSpeed, &terrainHeightAhead);

	if (terrainHeightAhead + _CurrentHeightOverTerrain < position._Y)
	{
		_CurrentVelocity -= SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	else
	{
		_CurrentVelocity += SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	//Modify the camera's height.
	position._Y += _CurrentVelocity * context->_DeltaTime;

	//Never let the camera go under the terrain.
	float terrainHeight;
	TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight);

	if (position._Y < CatalystBaseMath::Maximum<float>(terrainHeight + 1.0f, 1.0f))
	{
		position._Y = CatalystBaseMath::Maximum<float>(terrainHeight + 1.0f, 1.0f);

		_CurrentVelocity = 0.0f;
	}

	//Set the position of the camera.
	_Camera->SetPosition(position);

	return true;
}

/*
*	Updates the forward movement of the camera.
*/
void SoakCamera::UpdateForwardMovement(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the speed.
	if (_CurrentSpeed < _TargetSpeed)
	{
		_CurrentSpeed += SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	else
	{
		_CurrentSpeed -= SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	//Move the camera forward.
	_Camera->Move(_Camera->GetForwardVector() * _CurrentSpeed * context->_DeltaTime);
}

/*
*	Updates the rotation of the camera.
*/
void SoakCamera::UpdateRotation(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the current rotation.
	if (_CurrentRotation < _TargetRotation)
	{
		_CurrentRotation += SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	else
	{
		_CurrentRotation -= SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	//Rotate the camera.
	_Camera->Rotate(Vector3(0.0f, _CurrentRotation * context->_DeltaTime, 0.0f));
}

/*
*	Randomizes the properties of the soak camera.
*/
void SoakCamera::Randomize() NOEXCEPT
{
	//Randomize a new target speed.
	_TargetSpeed = CatalystBaseMath::RandomFloatInRange(2.0f, 200.0f);

	//Randomize the current height over terrain.
	_CurrentHeightOverTerrain = CatalystBaseMath::RandomFloatInRange(2.0f, 200.0f);

	//Randomize a new target rotation.
	_TargetRotation = CatalystBaseMath::RandomFloatInRange(-5.0f, 5.0f);

	//Randomize the time until the properties of the soak camera is randomized again.
	_RandomizeTime = CatalystBaseMath::RandomFloatInRange(1.0f, 100.0f);

	//Reset the timer.
	_RandomizeTimer = 0.0f;
}