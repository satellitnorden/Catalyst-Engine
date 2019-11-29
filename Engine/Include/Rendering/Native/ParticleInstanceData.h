#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class ParticleInstanceData final
{

public:

	//The position.
	Vector3<float> _Position;

	//The velocity.
	Vector3<float> _Velocity;

	//The scale.
	Vector2<float> _Scale;

	//The time.
	float _Time;

};