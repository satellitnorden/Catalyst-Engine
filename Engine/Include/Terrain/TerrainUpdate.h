#pragma once

//Core.
#include <Core/Core/CatalystCore.h>


//Terrain.
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>

class TerrainAddRootNodeUpdate final
{

public:

	//The grid point.
	GridPoint2 _GridPoint;

	//The patch information.
	TerrainPatchInformation _PatchInformation;

	//The patch render information.
	TerrainPatchRenderInformation _PatchRenderInformation;

};

class TerrainRemoveRootNodeUpdate final
{

public:

	//The index.
	uint8 _Index;

};

class TerrainRestoreNodeUpdate final
{

public:

	//The node to restore.
	TerrainQuadTreeNode *RESTRICT _Node;

	//The patch information.
	TerrainPatchInformation _PatchInformation;

	//The patch render information.
	TerrainPatchRenderInformation _PatchRenderInformation;

};

class TerrainSubdivideNodeUpdate final
{

public:

	//The node to subdivide.
	TerrainQuadTreeNode *RESTRICT _Node;

	//The patch informations.
	StaticArray<TerrainPatchInformation, 4> _PatchInformations;

	//The patch render informations.
	StaticArray<TerrainPatchRenderInformation, 4> _PatchRenderInformations;

};

class TerrainUpdate final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		Invalid,
		AddRootNode,
		RemoveRootNode,
		RestoreNode,
		SubdivideNode,
	};

	//The type.
	Type _Type{ Type::Invalid };

	union
	{
		//The add root node update.
		TerrainAddRootNodeUpdate _AddRootNodeUpdate;

		//The remove root node update.
		TerrainRemoveRootNodeUpdate _RemoveRootNodeUpdate;

		//The restore node update.
		TerrainRestoreNodeUpdate _RestoreNodeUpdate;

		//The subdivide node update.
		TerrainSubdivideNodeUpdate _SubdivideNodeUpdate;
	};

	/*
	*	Default constructor
	*/
	TerrainUpdate() NOEXCEPT
	{

	}

};