#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Memory.
#include <Memory/PoolAllocator.h>

class TerrainQuadTreeNode final
{

public:

	//The depth.
	uint8 _Depth{ 0 };

	//Denotes whether or not this node is subdivided.
	bool _Subdivided{ false };

	//The patch identifier.
	uint64 _Identifier{ UINT64_MAXIMUM };

	//The child nodes.
	TerrainQuadTreeNode *RESTRICT _ChildNodes{ nullptr };

	//The minimum world position of this node.
	Vector2<float> _Minimum;

	//The maximum world position of this node.
	Vector2<float> _Maximum;

	/*
	*	Returns whether or not a world position is within this node.
	*/
	bool IsWithin(const Vector3<float> &worldPosition) const NOEXCEPT
	{
		return	worldPosition._X >= _Minimum._X
				&& worldPosition._X < _Maximum._X
				&& worldPosition._Z >= _Minimum._Y
				&& worldPosition._Z < _Maximum._Y;
	}

};