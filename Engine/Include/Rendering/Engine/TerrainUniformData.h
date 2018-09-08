#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

class TerrainUniformData final
{

public:

	//The terrain first layer displacement height.
	float _TerrainFirstLayerDisplacementHeight;

	//The terrain second layer displacement height.
	float _TerrainSecondLayerDisplacementHeight;

	//The terrain third layer displacement height.
	float _TerrainThirdLayerDisplacementHeight;

	//The terrain fourth layer displacement height.
	float _TerrainFourthLayerDisplacementHeight;

	//The terrain fifth layer displacement height.
	float _TerrainFifthLayerDisplacementHeight;

	//The terrain height.
	float _TerrainHeight;

	//The terrain size.
	float _TerrainSize;

	//The terrain texture tiling factor.
	float _TerrainTextureTilingFactor;

	//The terrain position.
	Vector3 _TerrainPosition;

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
		_TerrainFirstLayerDisplacementHeight(initialTerrainFirstLayerDisplacementHeight),
		_TerrainSecondLayerDisplacementHeight(initialTerrainSecondLayerDisplacementHeight),
		_TerrainThirdLayerDisplacementHeight(initialTerrainThirdLayerDisplacementHeight),
		_TerrainFourthLayerDisplacementHeight(initialTerrainFourthLayerDisplacementHeight),
		_TerrainFifthLayerDisplacementHeight(initialTerrainFifthLayerDisplacementHeight),
		_TerrainHeight(initialTerrainHeight),
		_TerrainSize(initialTerrainSize),
		_TerrainTextureTilingFactor(initialTerrainTextureTilingFactor),
		_TerrainPosition(initialTerrainPosition)
	{

	}

};