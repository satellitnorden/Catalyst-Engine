#pragma once

//Core.
#include <Core/EngineCore.h>

//Forward declarations.
class CameraEntity;
class DirectionalLightEntity;

class MaximGameSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MaximGameSystem);

	/*
	*	Default constructor.
	*/
	MaximGameSystem() NOEXCEPT { }

	/*
	*	Initializes the Maxim game system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the Maxim game system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Releases the Maxim game system.
	*/
	void ReleaseSystem() NOEXCEPT;

private:

	//The camera.
	CameraEntity * RESTRICT camera;

	//The sun.
	DirectionalLightEntity *RESTRICT sun;

};