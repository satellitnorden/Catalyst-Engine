//Header file.
#include <Rendering/Native/CameraSystem.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/PhysicsSystem.h>

/*
*	Initializes the camera system.
*/
void CameraSystem::Initialize() NOEXCEPT
{
	//Create the default camera.
	_DefaultCamera = CreateCamera();

	//Set the aperture.
	_DefaultCamera->SetAperture(0.5f);

	//Set automatic focal distance.
	_DefaultCamera->SetAutomaticFocalDistance(true);
}

/*
*	Updates the camera system during the render update phase.
*/
void CameraSystem::RenderUpdate() NOEXCEPT
{
	//Is the current camera using automatic focal distance?
	if (GetCurrentCamera()->GetAutomaticFocalDistance())
	{
		//Construct the ray.
		Ray ray;

		ray.SetOrigin(GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition());
		ray.SetDirection(RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(0.5f, 0.5f)));

		//Construct the configuration.
		RaycastConfiguration configuration;

		configuration._PhysicsChannels = PhysicsChannel::ALL;
		configuration._MaximumHitDistance = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance;
		configuration._TerrainRayMarchStep = 1.0f;

		//Cast the ray!
		RaycastResult result;
		PhysicsSystem::Instance->CastRay(ray, configuration, &result);

		//Did it hit?
		if (result._HasHit)
		{
			GetCurrentCamera()->SetFocalDistance(result._HitDistance);
		}
	}
}

/*
*	Creates a new camera.
*/
RESTRICTED NO_DISCARD Camera *const RESTRICT CameraSystem::CreateCamera() NOEXCEPT
{
	Camera *const RESTRICT new_camera{ new (MemorySystem::Instance->TypeAllocate<Camera>()) Camera() };

	new_camera->SetFarPlane(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance * 2.0f);

	return new_camera;
}

/*
*	Destroys a camera.
*/
void CameraSystem::DestroyCamera(Camera *const RESTRICT camera) NOEXCEPT
{
	if (_CurrentCamera == camera)
	{
		_CurrentCamera = nullptr;
	}

	MemorySystem::Instance->TypeFree<Camera>(camera);
}