//Header file.
#include <Rendering/Native/CameraSystem.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
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
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
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
	constexpr StaticArray<Vector2<float32>, 16> JITTER_SAMPLES
	{
		Vector2<float32>(HaltonSequence::Generate(1, 2), HaltonSequence::Generate(1, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(2, 2), HaltonSequence::Generate(2, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(3, 2), HaltonSequence::Generate(3, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(4, 2), HaltonSequence::Generate(4, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(5, 2), HaltonSequence::Generate(5, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(6, 2), HaltonSequence::Generate(6, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(7, 2), HaltonSequence::Generate(7, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(8, 2), HaltonSequence::Generate(8, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(9, 2), HaltonSequence::Generate(9, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(10, 2), HaltonSequence::Generate(10, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(11, 2), HaltonSequence::Generate(11, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(12, 2), HaltonSequence::Generate(12, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(13, 2), HaltonSequence::Generate(13, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(14, 2), HaltonSequence::Generate(14, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(15, 2), HaltonSequence::Generate(15, 3)) * 2.0f - 1.0f,
		Vector2<float32>(HaltonSequence::Generate(16, 2), HaltonSequence::Generate(16, 3)) * 2.0f - 1.0f
	};

	//Calculate the previous camera/projection matrix.
	const Matrix4x4 previous_camera_matrix{ Matrix4x4::LookAt(_PreviousCameraWorldTransform.GetRelativePosition(GetCurrentCamera()->GetWorldTransform().GetCell()), _PreviousCameraWorldTransform.GetRelativePosition(GetCurrentCamera()->GetWorldTransform().GetCell()) + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_PreviousCameraWorldTransform.GetRotation().ToEulerAngles()), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_PreviousCameraWorldTransform.GetRotation().ToEulerAngles())) };
	Matrix4x4 previous_projection_matrix{ *GetCurrentCamera()->GetProjectionMatrix() };
	previous_projection_matrix._Matrix[2]._X += _CameraUniformData._CurrentFrameJitter._X;
	previous_projection_matrix._Matrix[2]._Y += _CameraUniformData._CurrentFrameJitter._Y;

	//Update the jitter.
	Vector2<float32> current_jitter;

	if (RenderingSystem::Instance->IsTakingScreenshot())
	{
		current_jitter = (Vector2<float32>(CatalystRandomMath::RandomFloat(), CatalystRandomMath::RandomFloat()) * 2.0f - 1.0f) * RenderingSystem::Instance->GetInverseScaledResolution(0);
	}

	else if (	(RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode() == RenderingConfiguration::AntiAliasingMode::TEMPORAL
				|| RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode() == RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE_PLUS_TEMPORAL)
				&& 
				(RenderingSystem::Instance->GetCurrentRenderingPath() == RenderingPath::DEFAULT
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

	//Update the camera uniform data.
	_CameraUniformData._WorldToClipMatrix = *GetCurrentCamera()->GetViewMatrix();
	_CameraUniformData._WorldToCameraMatrix = *GetCurrentCamera()->GetCameraMatrix();
	_CameraUniformData._PreviousWorldToClipMatrix = previous_projection_matrix * previous_camera_matrix;
	_CameraUniformData._InverseWorldToCameraMatrix = *GetCurrentCamera()->GetInverseCameraMatrix();
	_CameraUniformData._InverseCameraToClipMatrix = *GetCurrentCamera()->GetInverseProjectionMatrix();

	_CameraUniformData._CameraWorldPosition = GetCurrentCamera()->GetWorldTransform().GetLocalPosition();
	_CameraUniformData._CameraForwardVector = GetCurrentCamera()->GetForwardVector();

	/*
	*	Since we're jittering in NDC space, which is in the range [-1.0f, 1.0f],
	*	halve it since we're using this in screen space.
	*/
	_CameraUniformData._CurrentFrameJitter = current_jitter * 0.5f;

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