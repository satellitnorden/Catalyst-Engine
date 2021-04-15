#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class ModelSimulationConfiguration final
{

public:

	//Denotes whether or not to simulate physics.
	bool _SimulatePhysics{ false };

	//The initial velocity.
	Vector3<float32> _InitialVelocity{ 0.0f, 0.0f, 0.0f };

};