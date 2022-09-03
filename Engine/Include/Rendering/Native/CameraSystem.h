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

	//The default camera.
	Camera *RESTRICT _DefaultCamera{ nullptr };

	//The current camera.
	Camera *RESTRICT _CurrentCamera{ nullptr };

};