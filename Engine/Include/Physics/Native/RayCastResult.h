#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Core/Entity.h>

class DynamicModelRaycastResult final
{

public:

	//The entity.
	Entity *RESTRICT _Entity;

};

class StaticModelRaycastResult final
{

public:

	//The entity.
	Entity *RESTRICT _Entity;

};

class TerrainRaycastResult final
{

public:

	//The entity.
	Entity* RESTRICT _Entity;

};

class RaycastResult final
{

public:

	//Denotes whether or not there was a hit.
	bool _HasHit;

	//The hit distance.
	float32 _HitDistance;

	//The entity (can be nullptr if nothing was hit or if an entity was not hit).
	Entity *RESTRICT _Entity;

};