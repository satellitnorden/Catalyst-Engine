#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class ModelRaycastResult final
{

public:

	//Empty. (:

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

		MODEL,
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
		//The model raycast result, if a model was hit.
		ModelRaycastResult _ModelRaycastResult;

		//The terrain raycast result, if the terrain was hit.
		TerrainRaycastResult _TerrainRaycastResult;
	};

};