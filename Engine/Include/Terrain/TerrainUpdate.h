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

class TerrainUpdate final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		Invalid,
		AddRootNode,
		RemoveRootNode,
	};

	//The type.
	Type _Type{ Type::Invalid };

	union
	{
		//The add root node update.
		TerrainAddRootNodeUpdate _AddRootNodeUpdate;

		//The remove root node update.
		TerrainRemoveRootNodeUpdate _RemoveRootNodeUpdate;
	};

	/*
	*	Default constructor
	*/
	TerrainUpdate() NOEXCEPT
	{

	}

};