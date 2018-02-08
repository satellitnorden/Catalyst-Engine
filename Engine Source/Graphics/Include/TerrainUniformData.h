#pragma once

//Engine core.
#include <TerrainUniformData.h>

//Math.
#include <Vector3.h>

class TerrainUniformData final
{

public:

	/*
	*	Default constructor.
	*/
	TerrainUniformData() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	TerrainUniformData(const float initialTerrainDisplacementHeight, const float initialTerrainHeight, const float initialTerrainSize, const Vector3 &initialTerrainPosition) NOEXCEPT
		:
		terrainDisplacementHeight(initialTerrainDisplacementHeight),
		terrainHeight(initialTerrainHeight),
		terrainSize(initialTerrainSize),
		terrainPosition(initialTerrainPosition)
	{

	}

	//The terrain displacement height.
	float terrainDisplacementHeight;

	//The terrain height.
	float terrainHeight;

	//The terrain size.
	float terrainSize;

	//Padding.
	float padding1{ 0.0f };

	//The terrain position.
	Vector3 terrainPosition;

};