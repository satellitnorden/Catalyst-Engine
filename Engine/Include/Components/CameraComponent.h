#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector3.h>

//Physics.
#include <Physics/PhysicalConstants.h>

class CameraComponent final
{

public:

	//The position.
	Vector3 _Position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3 _Rotation{ 0.0f, 0.0f, 0.0f };

	//The field of view in degrees.
	float _FieldOfViewDegrees{ 60.0f };

	//The field of view in radians.
	float _FieldOfViewRadians{ CatalystBaseMath::DegreesToRadians(60.0f) };

	//The near plane.
	float _NearPlane{ 0.1f };

	//The far plane.
	float _FarPlane{ PHYSICAL_CONSTANT_VIEW_DISTANCE };

};