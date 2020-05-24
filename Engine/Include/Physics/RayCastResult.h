#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

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

	//Empty. (:

};

class RaycastResult final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		NONE,

		DYNAMIC_MODEL,
		STATIC_MODEL,
		TERRAIN
	};

	//Denotes whether or not there was a hit.
	bool _HasHit;

	//The hit distance.
	float32 _HitDistance;

	//Denotes the type of entity hit.
	Type _Type;

	union
	{
		//The dynamic model raycast result, if a dynamic model was hit.
		DynamicModelRaycastResult _DynamicModelRaycastResult;

		//The static model raycast result, if a static model was hit.
		StaticModelRaycastResult _StaticModelRaycastResult;

		//The terrain raycast result, if the terrain was hit.
		TerrainRaycastResult _TerrainRaycastResult;
	};

};