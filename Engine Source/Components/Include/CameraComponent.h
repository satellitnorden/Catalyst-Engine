#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

//Physics.
#include <PhysicalConstants.h>

class CameraComponent
{

public:

	//The position.
	Vector3 position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3 rotation{ 0.0f, 0.0f, 0.0f };

	//The field of view.
	float fieldOfView{ 60.0f };

	//The near plane.
	float nearPlane{ 0.1f };

	//The far plane.
	float farPlane{ PHYSICAL_CONSTANT_VIEW_DISTANCE };

};