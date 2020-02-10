#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

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

		TERRAIN
	};

	//Denotes whether or not there was a hit.
	bool _HasHit;

	//The hit distance.
	float _HitDistance;

	//Denotes the type of entity hit.
	Type _Type;

	union
	{
		//The terrain raycast result, if terrain was hit.
		TerrainRaycastResult _TerrainRaycastResult;
	};

};