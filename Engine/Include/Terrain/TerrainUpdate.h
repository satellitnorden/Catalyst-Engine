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

	//The identifier.
	uint64 _Identifier;

	//The patch informations.
	StaticArray<TerrainPatchInformation, 4> _PatchInformations;

	//The patch render informations.
	StaticArray<TerrainPatchRenderInformation, 4> _PatchRenderInformations;

};

class TerrainRemoveNodeUpdate final
{

public:


};

class TerrainAddNodeUpdate final
{

public:


};

class TerrainUpdate final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		CombineNode, //TODO: Remove this!
		SubdivideNode, //TODO: Remove this!

		REMOVE_ROOT_NODE,
		ADD_ROOT_NODE,
		REMOVE_NODE,
		ADD_NODE
	};

	//The type.
	Type _Type;

	union
	{
		//The combine node update.
		TerrainCombineNodeUpdate _CombineNodeUpdate; //TODO: Remove this!

		//The subdivide node update.
		TerrainSubdivideNodeUpdate _SubdivideNodeUpdate; //TODO: Remove this!

		//The remove root node update.
		TerrainRemoveRootNodeUpdate _RemoveRootNodeUpdate;

		//The add root node update.
		TerrainAddRootNodeUpdate _AddRootNodeUpdate;

		//The remove node update.
		TerrainRemoveNodeUpdate _RemoveNoteUpdate;

		//The add node update.
		TerrainAddNodeUpdate _AddNoteUpdate;
	};

	//The borders updates.
	DynamicArray<Pair<uint64, int32>> _BordersUpdates;

	/*
	*	Default constructor
	*/
	TerrainUpdate() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	TerrainUpdate(const TerrainUpdate& other) NOEXCEPT
	{
		_Type = other._Type;

		switch (other._Type)
		{
			case Type::REMOVE_ROOT_NODE:
			{
				_RemoveRootNodeUpdate = other._RemoveRootNodeUpdate;

				break;
			}

			case Type::ADD_ROOT_NODE:
			{
				_AddRootNodeUpdate = other._AddRootNodeUpdate;

				break;
			}

			case Type::CombineNode:
			{
				_CombineNodeUpdate = other._CombineNodeUpdate;

				break;
			}

			case Type::SubdivideNode:
			{
				_SubdivideNodeUpdate = other._SubdivideNodeUpdate;

				break;
			}
		}

		_BordersUpdates = other._BordersUpdates;
	}

	/*
	*	Copy operator overload.
	*/
	void operator=(const TerrainUpdate& other) NOEXCEPT
	{
		new (this) TerrainUpdate(other);
	}

};