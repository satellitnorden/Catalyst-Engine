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

	//Some padding.
	Padding<12> _Padding;

};