//Header file.
#include <Rendering/Native/CameraSystem.h>

//Math.
#include <Math/Noise/HaltonSequence.h>

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
	//Register the uniform data.
	RenderingSystem::Instance->GetUniformBufferManager()->RegisterUniformBuffer
	(
		HashString("Camera"),
		&_CameraUniformData,
		sizeof(CameraUniformData)
	);

	//Create the default camera.
	_DefaultCamera = CreateCamera();
}

/*
*	Updates the camera system during the render update phase.
*/
void CameraSystem::RenderUpdate() NOEXCEPT
{
	//Define constants.
	constexpr float JITTER_SAMPLE_MULTIPLIER{ 0.5f };
	constexpr StaticArray<Vector2<float>, 16> JITTER_SAMPLES
	{
		(Vector2<float>(HaltonSequence::Generate(0, 3), HaltonSequence::Generate(1, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(2, 3), HaltonSequence::Generate(3, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(4, 3), HaltonSequence::Generate(5, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(6, 3), HaltonSequence::Generate(7, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(8, 3), HaltonSequence::Generate(9, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(10, 3), HaltonSequence::Generate(11, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(12, 3), HaltonSequence::Generate(13, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(14, 3), HaltonSequence::Generate(15, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(16, 3), HaltonSequence::Generate(17, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(18, 3), HaltonSequence::Generate(19, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(20, 3), HaltonSequence::Generate(21, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(22, 3), HaltonSequence::Generate(23, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(24, 3), HaltonSequence::Generate(25, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(26, 3), HaltonSequence::Generate(27, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(28, 3), HaltonSequence::Generate(29, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(30, 3), HaltonSequence::Generate(31, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER
	};

	//Update the jitter.
	Vector2<float32> current_jitter;

	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode() == RenderingConfiguration::AntiAliasingMode::TEMPORAL
		&& (RenderingSystem::Instance->GetCurrentRenderingPath() == RenderingPath::DEFAULT
		|| RenderingSystem::Instance->GetCurrentRenderingPath() == RenderingPath::PATH_TRACING))
	{
		current_jitter = JITTER_SAMPLES[_CurrentJitterIndex] * RenderingSystem::Instance->GetInverseScaledResolution(0);
	}

	else
	{
		current_jitter = Vector2<float32>(0.0f, 0.0f);
	}

	_CurrentJitterIndex = _CurrentJitterIndex == JITTER_SAMPLES.Size() - 1 ? 0 : _CurrentJitterIndex + 1;

	GetCurrentCamera()->SetProjectionMatrixJitter(current_jitter);

	//Calculate the previous camera matrix.
	const Matrix4x4 previous_camera_matrix{ Matrix4x4::LookAt(_PreviousCameraWorldTransform.GetRelativePosition(GetCurrentCamera()->GetWorldTransform().GetCell()), _PreviousCameraWorldTransform.GetRelativePosition(GetCurrentCamera()->GetWorldTransform().GetCell()) + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_PreviousCameraWorldTransform.GetRotation().ToEulerAngles()), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_PreviousCameraWorldTransform.GetRotation().ToEulerAngles()))};

	//Update the camera uniform data.
	_CameraUniformData._WorldToClipMatrix = *GetCurrentCamera()->GetViewMatrix();
	_CameraUniformData._WorldToCameraMatrix = *GetCurrentCamera()->GetCameraMatrix();
	_CameraUniformData._PreviousWorldToClipMatrix = *GetCurrentCamera()->GetProjectionMatrix() * previous_camera_matrix;
	_CameraUniformData._InverseWorldToCameraMatrix = *GetCurrentCamera()->GetInverseCameraMatrix();
	_CameraUniformData._InverseCameraToClipMatrix = *GetCurrentCamera()->GetInverseProjectionMatrix();

	_CameraUniformData._CameraWorldPosition = GetCurrentCamera()->GetWorldTransform().GetLocalPosition();
	_CameraUniformData._CameraForwardVector = GetCurrentCamera()->GetForwardVector();

	_CameraUniformData._CurrentFrameJitter = current_jitter;

	_CameraUniformData._NearPlane = GetCurrentCamera()->GetNearPlane();
	_CameraUniformData._FarPlane = GetCurrentCamera()->GetFarPlane();

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

	//Update the previous camera world transform.
	_PreviousCameraWorldTransform = GetCurrentCamera()->GetWorldTransform();
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