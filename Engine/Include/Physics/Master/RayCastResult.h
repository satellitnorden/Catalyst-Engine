#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Vector3.h>

class RayCastResult final
{

public:

	//Denotes whether or not there was a hit.
	bool _HasHit;

	//The hit position.
	Vector3<float> _HitPosition;

	//The hit entity.
	Entity *RESTRICT _HitEntity;

};