#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class PhysicsComponent final
{

public:

	//Denotes whether or not to simulate physics.
	bool _SimulatePhysics;

	//The mass.
	float _Mass;

	//The velocity.
	Vector3<float> _Velocity;

};