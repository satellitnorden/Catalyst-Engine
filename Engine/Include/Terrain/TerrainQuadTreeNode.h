#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Memory.
#include <Memory/PoolAllocator.h>

class TerrainQuadTreeNode final
{

public:

	//The depth.
	uint8 _Depth;

	//Denotes whether or not this node is subdivided.
	bool _Subdivided;

	//The patch identifier.
	uint64 _Identifier;

	//The child nodes.
	TerrainQuadTreeNode *RESTRICT _ChildNodes;

	//The minimum world position of this node.
	Vector2 _Minimum;

	//The maximum world position of this node.
	Vector2 _Maximum;

	/*
	*	Returns whether or not a world position is within this node.
	*/
	bool IsWithin(const Vector3 &worldPosition) NOEXCEPT
	{
		return	worldPosition._X >= _Minimum._X
				&& worldPosition._X < _Maximum._X
				&& worldPosition._Z >= _Minimum._Y
				&& worldPosition._Z < _Maximum._Y;
	}

	/*
	*	Subdivides the node.
	*/
	void Subdivide() NOEXCEPT
	{
		_Subdivided = true;

		_ChildNodes = static_cast<TerrainQuadTreeNode *RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(TerrainQuadTreeNode) * 4));

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_ChildNodes[i]._Depth = _Depth + 1;
			_ChildNodes[i]._Subdivided = false;
			_ChildNodes[i]._ChildNodes = nullptr;
		}
	}

};