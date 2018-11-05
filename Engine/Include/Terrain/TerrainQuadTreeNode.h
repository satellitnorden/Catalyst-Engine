#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TerrainQuadTreeNode final
{

public:

	//Denotes the depth of this node.
	uint8 _Depth;

	//Denotes whether or not this node is subdivided.
	bool _Subdivided;

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

};