#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

class TerrainQuadTreeNode final
{

public:

	//The depth.
	uint8 _Depth{ 0 };

	//The borders.
	uint32 _Borders;

	//The child nodes.
	DynamicArray<TerrainQuadTreeNode> _ChildNodes;

	//The minimum world position of this node.
	Vector2<float32> _Minimum;

	//The maximum world position of this node.
	Vector2<float32> _Maximum;

	//The axis-aligned bounding box for this terrain patch.
	AxisAlignedBoundingBox3D _AxisAlignedBoundingBox;

	//Denotes if this node is visible.
	bool _Visible;

	//The position.
	Vector2<float32> _Position;

	//The minimum height map coordinate.
	Vector2<float32> _MinimumHeightMapCoordinate;

	//The maximum height map coordinate.
	Vector2<float32> _MaximumHeightMapCoordinate;

	//The patch size.
	float32 _PatchSize;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE TerrainQuadTreeNode() NOEXCEPT
	{

	}

	/*
	*	Returns whether or not this node is subdivided.
	*/
	FORCE_INLINE NO_DISCARD bool IsSubdivided() const NOEXCEPT
	{
		return !_ChildNodes.Empty();
	}

	/*
	*	Returns whether or not a world position is within this node.
	*/
	FORCE_INLINE NO_DISCARD bool IsWithin(const Vector2<float32> &world_position) const NOEXCEPT
	{
		return	world_position._X >= _Minimum._X
				&& world_position._X < _Maximum._X
				&& world_position._Y >= _Minimum._Y
				&& world_position._Y < _Maximum._Y;
	}

};