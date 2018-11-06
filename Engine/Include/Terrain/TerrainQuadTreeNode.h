#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TerrainQuadTreeNode final
{

public:

	//The patch identifier.
	uint64 _Identifier;

	//Denotes whether or not this node is subdivided.
	bool _Subdivided;

	//The children.
	TerrainQuadTreeNode *RESTRICT _Children;

	//The minimum world position of this node.
	Vector2 _Minimum;

	//The maximum world position of this node.
	Vector2 _Maximum;

	/*
	*	Returns a pointer to the child nodes.
	*/
	RESTRICTED const TerrainQuadTreeNode *const RESTRICT GetChildNodes() const NOEXCEPT
	{
		return _Subdivided ? _Children : nullptr;
	}

	/*
	*	Subdivides this node.
	*/
	void Subdivide() NOEXCEPT
	{
		const float range{ _Maximum._X - _Minimum._X };

		const StaticArray<Vector2, 4> minimumOffsets
		{
			Vector2(range * 0.5f, range * 0.5f),
			Vector2(range * 0.5f, 0.0f),
			Vector2(0.0f, 0.0f),
			Vector2(0.0f, range * 0.5f)
		};

		const StaticArray<Vector2, 4> maximumOffsets
		{
			Vector2(0.0f, 0.0f),
			Vector2(0.0f, range * 0.5f),
			Vector2(range * 0.5f, range * 0.5f),
			Vector2(range * 0.5f, 0.0f)
		};

		if (_Children)
		{
			delete[] _Children;
		}

		_Children = new TerrainQuadTreeNode[4];

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			TerrainQuadTreeNode *const RESTRICT node{ &_Children[i] };

			node->_Subdivided = false;
			node->_Children = nullptr;
			node->_Minimum = _Minimum + minimumOffsets[i];
			node->_Maximum = _Maximum - maximumOffsets[i];
		}

		_Subdivided = true;
	}

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

};