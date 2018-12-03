//Header file.
#include <Player/SoakCamera.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
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
	//Register the soak camera for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);

	//Randomize.
	Randomize();

	//Constrain the viewer to the ground.
	Vector3<float> position{ Viewer::Instance->GetPosition() };

	float terrainHeight;

	if (TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
	{
		position._Y = terrainHeight;

		Viewer::Instance->SetPosition(position);
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

	//Update the altitude of the camera.
	UpdateAltitude(context);

	//Update the rotation of the camera.
	UpdateRotation(context);

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

	//Move the viewer forward.
	Viewer::Instance->Move(Viewer::Instance->GetForwardVector() * _CurrentSpeed * context->_DeltaTime);
}

/*
*		Updates the altitude of the camera.
*/
void SoakCamera::UpdateAltitude(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Get the current position of the viewer.
	Vector3<float> position{ Viewer::Instance->GetPosition() };

	//Look ahead at the terrain height at a position in front of the viewer to determine how the velocity should be affected.
	float terrainHeightAhead;
	TerrainSystem::Instance->GetTerrainHeightAtPosition(Viewer::Instance->GetPosition() + Viewer::Instance->GetForwardVector() * _CurrentSpeed, &terrainHeightAhead);

	if (position._Y > CatalystBaseMath::Maximum<float>(terrainHeightAhead + _TargetAltitude, 1.0f))
	{
		_CurrentUpwardsVelocity -= SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	else
	{
		_CurrentUpwardsVelocity += SoakCameraConstants::OPERATIONAL_SPEED * context->_DeltaTime;
	}

	//Modify the viewer's height.
	position._Y += _CurrentUpwardsVelocity * context->_DeltaTime;

	//Never let the viewer go under the terrain.
	float terrainHeight;
	TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight);

	if (position._Y < terrainHeight + 1.0f)
	{
		position._Y = terrainHeight + 1.0f;

		_CurrentUpwardsVelocity = CatalystBaseMath::Absolute(_CurrentUpwardsVelocity);
	}

	//Set the position of the viewer.
	Viewer::Instance->SetPosition(position);
}

/*
*	Updates the rotation of the camera.
*/
void SoakCamera::UpdateRotation(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the current rotation.
	if (_CurrentRotation < _TargetRotation)
	{
		_CurrentRotation += SoakCameraConstants::OPERATIONAL_SPEED * 0.1f * context->_DeltaTime;
	}

	else
	{
		_CurrentRotation -= SoakCameraConstants::OPERATIONAL_SPEED * 0.1f * context->_DeltaTime;
	}

	//Rotate the viewer.
	Viewer::Instance->Rotate(Vector3<float>(0.0f, _CurrentRotation * context->_DeltaTime, 0.0f));
}

/*
*	Randomizes the properties of the soak camera.
*/
void SoakCamera::Randomize() NOEXCEPT
{
	//Randomize a new target speed.
	_TargetSpeed = CatalystBaseMath::RandomFloatInRange(2.0f, 200.0f);

	//Randomize the target altitude.
	_TargetAltitude = CatalystBaseMath::RandomFloatInRange(2.0f, 200.0f);

	//Randomize a new target rotation.
	_TargetRotation = CatalystBaseMath::RandomFloatInRange(-2.5f, 2.5f);

	//Randomize the time until the properties of the soak camera is randomized again.
	_RandomizeTime = CatalystBaseMath::RandomFloatInRange(1.0f, 100.0f);

	//Reset the timer.
	_RandomizeTimer = 0.0f;
}