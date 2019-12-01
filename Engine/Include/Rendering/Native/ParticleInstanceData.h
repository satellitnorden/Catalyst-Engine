#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Vector.h>

class ParticleInstanceData final
{

public:

	//The position.
	Vector3<float> _Position;

	//The velocity.
	Vector3<float> _Velocity;

	//The size.
	Vector2<float> _Size;

	//The time.
	float _Time;

	//The random seed 1.
	float _RandomSeed1;

	//The random seed 2.
	float _RandomSeed2;

	//The random seed 3.
	float _RandomSeed3;

};