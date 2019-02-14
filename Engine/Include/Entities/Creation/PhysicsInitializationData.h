#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/Vector3.h>

class PhysicsInitializationData final : public EntityInitializationData
{

public:

	//The initial velocity.
	Vector3<float> _InitialVelocity;

	//The mass.
	float _Mass;

};