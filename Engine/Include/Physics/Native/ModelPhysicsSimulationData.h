#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class ModelPhysicsSimulationData final
{

public:

	//The acceleration.
	Vector3<float32> _Acceleration;

	//The velocity.
	Vector3<float32> _Velocity;

	//The mass.
	float32 _Mass;

};