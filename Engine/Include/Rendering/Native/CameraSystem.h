#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Camera.h>

class CameraSystem final
{

public:

	/*
	*	Initializes the camera system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the camera system during the render update phase.
	*/
	void RenderUpdate() NOEXCEPT;

	/*
	*	Creates a new camera.
	*/
	RESTRICTED NO_DISCARD Camera *const RESTRICT CreateCamera() NOEXCEPT;

	/*
	*	Returns the current camera.
	*/
	FORCE_INLINE NO_DISCARD Camera *const RESTRICT GetCurrentCamera() NOEXCEPT
	{
		return _CurrentCamera ? _CurrentCamera : _DefaultCamera;
	}

	/*
	*	Sets the current camera.
	*/
	FORCE_INLINE void SetCurrentCamera(Camera *const RESTRICT value) NOEXCEPT
	{
		_CurrentCamera = value;
	}

	/*
	*	Destroys a camera.
	*/
	void DestroyCamera(Camera *const RESTRICT camera) NOEXCEPT;

private:

	/*
	*	Camera uniform data class definition.
	*/
	class CameraUniformData final
	{

	public:

		Matrix4x4 _WorldToClipMatrix;
		Matrix4x4 _WorldToCameraMatrix;
		Matrix4x4 _PreviousWorldToClipMatrix;
		Matrix4x4 _InverseWorldToCameraMatrix;
		Matrix4x4 _InverseCameraToClipMatrix;

		Vector4<float32> _CameraWorldPosition; //Actually vec3, but needs padding.
		Vector4<float32> _CameraForwardVector; //Actually vec3, but needs padding.

		Vector2<float32> _CurrentFrameJitter;

		float32 _NearPlane;
		float32 _FarPlane;
	};

	//The camera uniform data.
	CameraUniformData _CameraUniformData;

	//The current jitter index.
	uint8 _CurrentJitterIndex{ 0 };

	//The previous camera world transform.
	WorldTransform _PreviousCameraWorldTransform;

	//The previous projection matrix.
	Matrix4x4 _PreviousProjectionMatrix;

	//The default camera.
	Camera *RESTRICT _DefaultCamera{ nullptr };

	//The current camera.
	Camera *RESTRICT _CurrentCamera{ nullptr };

};