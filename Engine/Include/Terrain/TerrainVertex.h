#pragma once

//Terrain.
#include <Terrain/TerrainCore.h>

#if NEW_TERRAIN_SYSTEM

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class TerrainVertex final
{

public:

	//The position.
	Vector3<float> _Position;

};

#else

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class TerrainVertex final
{

public:

	//The position.
	Vector2<float> _Position;

	//The borders.
	int32 _Borders;

};

#endif