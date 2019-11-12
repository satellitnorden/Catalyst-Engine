#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Pair.h>

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

class TerrainCombineNodeUpdate final
{

public:

	//The node to restore.
	TerrainQuadTreeNode *RESTRICT _Node;

	//The patch information.
	TerrainPatchInformation _PatchInformation;

	//The patch render information.
	TerrainPatchRenderInformation _PatchRenderInformation;

	//The identifiers for the patch informations to destroy.
	StaticArray<uint64, 4> _PatchInformationIdentifiers;

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
		CombineNode,
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

		//The combine node update.
		TerrainCombineNodeUpdate _CombineNodeUpdate;

		//The subdivide node update.
		TerrainSubdivideNodeUpdate _SubdivideNodeUpdate;
	};

	//The borders updates.
	DynamicArray<Pair<uint64, int32>> _BordersUpdates;

	/*
	*	Default constructor
	*/
	TerrainUpdate() NOEXCEPT
	{

	}

};