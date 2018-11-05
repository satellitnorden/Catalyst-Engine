#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Terrain.
#include <Terrain/TerrainQuadTreeNode.h>

class TerrainQuadTree final
{

public:

	//The root nodes.
	StaticArray<TerrainQuadTreeNode, 9> _RootNodes;

	/*
	*	Initializes the terrain quad tree.
	*/
	void Initialize() NOEXCEPT
	{
		//Set the initial properties of all root nodes.
		for (TerrainQuadTreeNode &rootNode : _RootNodes)
		{
			rootNode._Depth = 0;
			rootNode._Subdivided = false;
		}
	}

};