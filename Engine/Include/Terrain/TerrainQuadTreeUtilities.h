#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainQuadTree.h>

class TerrainQuadTreeUtilities
{

public:

	/*
	*	Given a node, return it's middle point.
	*/
	static Vector3 MiddlePoint(const TerrainQuadTreeNode &node) NOEXCEPT
	{
		return Vector3(	node._Minimum._X + ((node._Maximum._X - node._Minimum._X) * 0.5f),
						0.0f,
						node._Minimum._Y + ((node._Maximum._Y - node._Minimum._Y) * 0.5f));
	}

	/*
	*	Given a quad tree and a node, returning the neighboring nodes in the order upper, right, lower, left.
	*/
	static void NeighboringNodes(const TerrainQuadTree &quadTree, const TerrainQuadTreeNode &node, StaticArray<const TerrainQuadTreeNode *RESTRICT, 4> *const RESTRICT neighboringNodes) NOEXCEPT
	{
		//Calculate the middle point of the node.
		const Vector3 middlePoint{ MiddlePoint(node) };

		//Calculate the patch size multiplier.
		const float patchSizeMultiplier{ PatchSizeMultiplier(node) };

		//Calculate the positions.
		const StaticArray<const Vector3, 4> positions
		{
			middlePoint + Vector3(0.0f, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, 0.0f),
			middlePoint + Vector3(0.0f, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, 0.0f)
		};

		//Find the node with the highest depth for each position.
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			neighboringNodes->At(i) = FindHighestNode(quadTree, positions[i]);
		}
	}

	/*
	*	Given a node, return it's resolution multiplier.
	*/
	static uint8 ResolutionMultiplier(const uint8 depth) NOEXCEPT
	{
		return CatalystBaseMath::Maximum<uint8>(TerrainConstants::TERRAIN_QUAD_TREE_MAX_DEPTH - depth, 1);
	}

	/*
	*	Given a node, return it's patch size multiplier.
	*/
	static float PatchSizeMultiplier(const TerrainQuadTreeNode &node) NOEXCEPT
	{
		return 1.0f / static_cast<float>(1 << (node._Depth));
	}

	/*
	*	Given a node and a position, returns if the node should be combined.
	*/
	static bool ShouldBeCombined(const TerrainQuadTreeNode &node, const Vector3 &position) NOEXCEPT
	{
		if (node._Subdivided && node._ChildNodes->_Subdivided)
		{
			return false;
		}

		else
		{
			const Vector3 middlePoint{ MiddlePoint(node) };
			const float length{ CatalystBaseMath::Maximum<float>(CatalystBaseMath::Absolute(middlePoint._X - position._X), CatalystBaseMath::Absolute(middlePoint._Z - position._Z)) };

			return length > TerrainConstants::TERRAIN_PATCH_SIZE * PatchSizeMultiplier(node) * 0.9f;
		}
	}

	/*
	*	Given a node and a position, returns if the node should be subdivided.
	*/
	static bool ShouldBeSubdivided(const TerrainQuadTreeNode &node, const Vector3 &position) NOEXCEPT
	{
		const Vector3 middlePoint{ MiddlePoint(node) };
		const float length{ CatalystBaseMath::Maximum<float>(CatalystBaseMath::Absolute(middlePoint._X - position._X), CatalystBaseMath::Absolute(middlePoint._Z - position._Z)) };

		return	node._Depth < TerrainConstants::TERRAIN_QUAD_TREE_MAX_DEPTH
				&& length <= TerrainConstants::TERRAIN_PATCH_SIZE * PatchSizeMultiplier(node) * 0.9f;
	}

private:

	/*
	*	Given a quad tree and a position, find the node with the highest depth within the quad tree.
	*/
	RESTRICTED static const TerrainQuadTreeNode *const RESTRICT FindHighestNode(const TerrainQuadTree &quadTree, const Vector3 &position) NOEXCEPT
	{
		for (const TerrainQuadTreeNode &rootNode : quadTree._RootNodes)
		{
			if (const TerrainQuadTreeNode *const RESTRICT node{ FindHighestNode(rootNode, position) })
			{
				return node;
			}
		}

		return nullptr;
	}

	/*
	*	Given a node and a position, find the node with the highest depth within the node.
	*/
	RESTRICTED static const TerrainQuadTreeNode *const RESTRICT FindHighestNode(const TerrainQuadTreeNode &node, const Vector3 &position) NOEXCEPT
	{
		if (node._Subdivided)
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (const TerrainQuadTreeNode *const RESTRICT childNode{ FindHighestNode(node._ChildNodes[i], position) })
				{
					return childNode;
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