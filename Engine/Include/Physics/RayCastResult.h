#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class RaycastResult final
{

public:

	//Denotes whether or not there was a hit.
	bool _HasHit;

	//The hit distance.
	float _HitDistance;

	//The hit entity.
	Entity *RESTRICT _HitEntity;

};