#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Vector3.h>

class PhysicsComponent final
{

public:

	//The velocity.
	Vector3<float> _Velocity;

	//The mass.
	float _Mass;

};