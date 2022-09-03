//Header file.
#include <Rendering/Native/CameraSystem.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/WorldTracingSystem.h>

/*
*	Initializes the camera system.
*/
void CameraSystem::Initialize() NOEXCEPT
{
	//Create the default camera.
	_DefaultCamera = CreateCamera();
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

		//Cast the ray!
		float32 hit_distance;

		//Did it hit?
		if (WorldTracingSystem::Instance->DistanceRay(ray, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance, &hit_distance, false))
		{
			GetCurrentCamera()->SetFocalDistance(hit_distance);
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