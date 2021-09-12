#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/Task.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

//Terrain.
#include <Terrain/TerrainQuadTreeNode.h>

class TerrainQuadTreeNodeUpdate final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		ADD_ROOT_NODE,
		COMBINE,
		SUBDIVIDE
	};

	//The type.
	Type _Type;

	//The task.
	Task _Task;

	union
	{
		struct
		{
			//The grid point.
			GridPoint2 _GridPoint;

			//The new node.
			TerrainQuadTreeNode _NewNode;
		} _AddRootNodeData;

		struct
		{
			//The node.
			TerrainQuadTreeNode *RESTRICT _Node;

			//The node.
			TerrainQuadTreeNode _NewNode;
		} _CombineData;

		struct
		{
			//The parent node.
			TerrainQuadTreeNode *RESTRICT _ParentNode;

			//The new nodes.
			StaticArray<TerrainQuadTreeNode, 4> _NewNodes;
		} _SubdivideData;
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE TerrainQuadTreeNodeUpdate(const Type initial_type) NOEXCEPT
		:
		_Type(initial_type)
	{
		switch (_Type)
		{
			case Type::ADD_ROOT_NODE:
			{
				new (&_AddRootNodeData._NewNode) TerrainQuadTreeNode();

				break;
			}

			case Type::COMBINE:
			{
				new (&_CombineData._NewNode) TerrainQuadTreeNode();

				break;
			}

			case Type::SUBDIVIDE:
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					new (&_SubdivideData._NewNodes[i]) TerrainQuadTreeNode();
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");
			}
		}
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~TerrainQuadTreeNodeUpdate() NOEXCEPT
	{
		switch (_Type)
		{
			case Type::ADD_ROOT_NODE:
			{
				_AddRootNodeData._NewNode.~TerrainQuadTreeNode();

				break;
			}

			case Type::COMBINE:
			{
				_CombineData._NewNode.~TerrainQuadTreeNode();

				break;
			}

			case Type::SUBDIVIDE:
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					_SubdivideData._NewNodes[i].~TerrainQuadTreeNode();
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");
			}
		}
	}

};