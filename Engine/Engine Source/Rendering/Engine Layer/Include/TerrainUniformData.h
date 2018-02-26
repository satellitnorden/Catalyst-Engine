#pragma once

//Engine core.
#include <EngineCore.h>

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
	TerrainUniformData(const float initialTerrainFirstLayerDisplacementHeight, const float initialTerrainSecondLayerDisplacementHeight, const float initialTerrainThirdLayerDisplacementHeight, const float initialTerrainFourthLayerDisplacementHeight, const float initialTerrainFifthLayerDisplacementHeight, const float initialTerrainHeight, const float initialTerrainSize, const float initialTerrainTextureTilingFactor, const Vector3 &initialTerrainPosition) NOEXCEPT
		:
		terrainFirstLayerDisplacementHeight(initialTerrainFirstLayerDisplacementHeight),
		terrainSecondLayerDisplacementHeight(initialTerrainSecondLayerDisplacementHeight),
		terrainThirdLayerDisplacementHeight(initialTerrainThirdLayerDisplacementHeight),
		terrainFourthLayerDisplacementHeight(initialTerrainFourthLayerDisplacementHeight),
		terrainFifthLayerDisplacementHeight(initialTerrainFifthLayerDisplacementHeight),
		terrainHeight(initialTerrainHeight),
		terrainSize(initialTerrainSize),
		terrainTextureTilingFactor(initialTerrainTextureTilingFactor),
		terrainPosition(initialTerrainPosition)
	{

	}

	//The terrain first layer displacement height.
	float terrainFirstLayerDisplacementHeight;

	//The terrain second layer displacement height.
	float terrainSecondLayerDisplacementHeight;

	//The terrain third layer displacement height.
	float terrainThirdLayerDisplacementHeight;

	//The terrain fourth layer displacement height.
	float terrainFourthLayerDisplacementHeight;

	//The terrain fifth layer displacement height.
	float terrainFifthLayerDisplacementHeight;

	//The terrain height.
	float terrainHeight;

	//The terrain size.
	float terrainSize;

	//The terrain texture tiling factor.
	float terrainTextureTilingFactor;

	//The terrain position.
	Vector3 terrainPosition;

};