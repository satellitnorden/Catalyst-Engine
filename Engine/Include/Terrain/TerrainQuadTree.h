#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Terrain.
#include <Terrain/TerrainQuadTreeNode.h>

class TerrainQuadTree final
{

public:

	//The root grid points.
	StaticArray<GridPoint2, 9> _RootGridPoints;

	//The root nodes.
	StaticArray<TerrainQuadTreeNode, 9> _RootNodes;

	/*
	*	Initializes the terrain quad tree.
	*/
	void Initialize() NOEXCEPT
	{
		//Set the initial properties of all root grid points.
		for (GridPoint2 &rootGridPoint : _RootGridPoints)
		{
			rootGridPoint._X = INT32_MAXIMUM;
			rootGridPoint._Y = INT32_MAXIMUM;
		}
	}

};