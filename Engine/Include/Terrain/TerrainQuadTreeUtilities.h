#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Components/TerrainComponent.h>

//Math.
#include <Math/General/Vector.h>

//Terrain.
#include <Terrain/TerrainQuadTree.h>

class TerrainQuadTreeUtilities
{

public:

	/*
	*	Given a node, return it's patch size multiplier.
	*/
	FORCE_INLINE static NO_DISCARD float32 PatchSizeMultiplier(const TerrainQuadTreeNode &node) NOEXCEPT
	{
		return 1.0f / static_cast<float32>(1 << (node._Depth));
	}

	/*
	*	Given a node, return it's middle point.
	*/
	FORCE_INLINE static NO_DISCARD Vector2<float32> MiddlePoint(const TerrainQuadTreeNode &node) NOEXCEPT
	{
		return Vector2<float32>(node._Minimum._X + ((node._Maximum._X - node._Minimum._X) * 0.5f),
								node._Minimum._Y + ((node._Maximum._Y - node._Minimum._Y) * 0.5f));
	}

	/*
	*	Given a node and a position, returns if the node should be combined.
	*/
	FORCE_INLINE static NO_DISCARD bool ShouldBeCombined(const uint32 patch_size, const uint8 maximum_subdivision_steps, const TerrainQuadTreeNode& node, const Vector3<float32>& position) NOEXCEPT
	{
		if (node.IsSubdivided() && node._ChildNodes.Data()->IsSubdivided())
		{
			return false;
		}

		else
		{
			const Vector2<float32> middle_point{ MiddlePoint(node) };
			const float32 length{ CatalystBaseMath::Maximum<float32>(CatalystBaseMath::Absolute(middle_point._X - position._X), CatalystBaseMath::Absolute(middle_point._Y - position._Z)) };

			return	node._Depth > maximum_subdivision_steps
					|| length > static_cast<float32>(patch_size) * PatchSizeMultiplier(node);
		}
	}

	/*
	*	Given a node and a position, returns if the node should be subdivided.
	*/
	FORCE_INLINE static NO_DISCARD bool ShouldBeSubdivided(const uint32 patch_size, const uint8 maximum_subdivision_steps, const TerrainQuadTreeNode &node, const Vector3<float32> &position) NOEXCEPT
	{
		const Vector2<float32> middle_point{ MiddlePoint(node) };
		const float32 length{ CatalystBaseMath::Maximum<float32>(CatalystBaseMath::Absolute(middle_point._X - position._X), CatalystBaseMath::Absolute(middle_point._Y - position._Z)) };

		return	node._Depth < maximum_subdivision_steps
				&& length <= static_cast<float32>(patch_size) * PatchSizeMultiplier(node);
	}

	/*
	*	Given a terrain component and a node, returning the neighboring nodes in the order upper, right, lower, left.
	*/
	FORCE_INLINE static void NeighboringNodes(const uint32 patch_size, const TerrainQuadTreeNode &root_node, const TerrainQuadTreeNode &node, StaticArray<const TerrainQuadTreeNode *RESTRICT, 4> *const RESTRICT neighboring_nodes) NOEXCEPT
	{
		//Calculate the middle point of the node.
		const Vector2<float32> middle_point{ MiddlePoint(node) };

		//Calculate the patch size multiplier.
		const float32 patch_size_multiplier{ PatchSizeMultiplier(node) };

		//Calculate the positions.
		const StaticArray<const Vector2<float32>, 4> positions
		{
			middle_point + Vector2<float32>(-static_cast<float32>(patch_size) * patch_size_multiplier, 0.0f),
			middle_point + Vector2<float32>(0.0f, -static_cast<float32>(patch_size) * patch_size_multiplier),
			middle_point + Vector2<float32>(static_cast<float32>(patch_size) * patch_size_multiplier, 0.0f),
			middle_point + Vector2<float32>(0.0f, static_cast<float32>(patch_size) * patch_size_multiplier),
		};

		//Find the node with the highest depth for each position.
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			neighboring_nodes->At(i) = FindHighestNode(root_node, positions[i]);
		}
	}

	/*
	*	Given a node and a position, find the node with the highest depth within the node.
	*/
	RESTRICTED static const TerrainQuadTreeNode *const RESTRICT FindHighestNode(const TerrainQuadTreeNode &node, const Vector2<float32> &position) NOEXCEPT
	{
		if (node.IsSubdivided())
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (const TerrainQuadTreeNode *const RESTRICT child_node{ FindHighestNode(node._ChildNodes[i], position) })
				{
					return child_node;
				}
			}
		}

		if (node.IsWithin(position))
		{
			return &node;
		}

		else
		{
			return nullptr;
		}
	}

};