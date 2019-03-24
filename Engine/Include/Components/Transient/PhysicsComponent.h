#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

class PhysicsComponent final
{

public:

	//The velocity.
	Vector3<float> _Velocity;

	//The mass.
	float _Mass;

	//The physics shape.
	PhysicsShape _PhysicsShape;

};