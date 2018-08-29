#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Physics.
#include <Physics/PhysicalConstants.h>

class CameraComponent final
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