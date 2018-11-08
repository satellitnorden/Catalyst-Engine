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
	*	Given a quad tree and a node, returns whether or not a node can be combined.
	*/
	static bool CanBeCombined(TerrainQuadTreeNode *const RESTRICT nodeToBeCombined, TerrainQuadTree *const RESTRICT quadTree) NOEXCEPT
	{
		//Calculate the middle point.
		const Vector3 middlePoint{	nodeToBeCombined->_Minimum._X + ((nodeToBeCombined->_Maximum._X - nodeToBeCombined->_Minimum._X) * 0.5f),
									0.0f,
									nodeToBeCombined->_Minimum._Y + ((nodeToBeCombined->_Maximum._Y - nodeToBeCombined->_Minimum._Y) * 0.5f) };

		//Calculate the patch size multiplier.
		const float patchSizeMultiplier{ PatchSizeMultiplier(*nodeToBeCombined) };

		//Calculate the positions.
		const StaticArray<const Vector3, 8> positions
		{
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, 0.0f),
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),

			middlePoint + Vector3(0.0f, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(0.0f, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),

			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, 0.0f),
			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
		};

		//Find the highest node.
		for (const Vector3 &position : positions)
		{
			if (TerrainQuadTreeNode *const RESTRICT node{ FindHighestNode(position, quadTree) })
			{
				if (nodeToBeCombined->_Depth < node->_Depth)
				{
					return false;
				}
			}
		}

		return true;
	}

	/*
	*	Given a node, return it's resolution multiplier.
	*/
	static uint8 ResolutionMultiplier(const float patchSizeMultiplier) NOEXCEPT
	{
		return CatalystBaseMath::Maximum<uint8>(static_cast<uint8>(static_cast<float>(TerrainConstants::TERRAIN_QUAD_TREE_MAX_DEPTH) * patchSizeMultiplier), 1);
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
		if (node._Subdivided)
		{
			if (node._ChildNodes[0]._Subdivided)
			{
				return false;
			}

			else
			{
				return !node.IsWithin(position);
			}
		}

		else
		{
			return false;
		}
	}

	/*
	*	Given a node and a position, returns if the node should be subdivided.
	*/
	static bool ShouldBeSubdivided(const TerrainQuadTreeNode &node, const Vector3 &position) NOEXCEPT
	{
		return node._Depth < TerrainConstants::TERRAIN_QUAD_TREE_MAX_DEPTH && node.IsWithin(position);
	}

	/*
	*	Given a node that should be subdivided, return if there exist a required node for that subdivision.
	*/
	RESTRICTED static TerrainQuadTreeNode *const RESTRICT SubdivisionRequirement(TerrainQuadTreeNode *const RESTRICT nodeToBeSubdivided, TerrainQuadTree *const RESTRICT quadTree) NOEXCEPT
	{
		//Calculate the middle point.
		const Vector3 middlePoint{	nodeToBeSubdivided->_Minimum._X + ((nodeToBeSubdivided->_Maximum._X - nodeToBeSubdivided->_Minimum._X) * 0.5f),
									0.0f,
									nodeToBeSubdivided->_Minimum._Y + ((nodeToBeSubdivided->_Maximum._Y - nodeToBeSubdivided->_Minimum._Y) * 0.5f) };

		//Calculate the patch size multiplier.
		const float patchSizeMultiplier{ PatchSizeMultiplier(*nodeToBeSubdivided) };

		//Calculate the positions.
		const StaticArray<const Vector3, 8> positions
		{
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, 0.0f),
			middlePoint + Vector3(-TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),

			middlePoint + Vector3(0.0f, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(0.0f, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),

			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, -TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, 0.0f),
			middlePoint + Vector3(TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier, 0.0f, TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier),
		};

		//Find the highest node.
		for (const Vector3 &position : positions)
		{
			if (TerrainQuadTreeNode *const RESTRICT node{ FindHighestNode(position, quadTree) })
			{
				if (nodeToBeSubdivided->_Depth > node->_Depth)
				{
					return node;
				}
			}
		}

		return nullptr;
	}

private:

	/*
	*	Given a position and a quad tree, find the node with the highest depth that the position is within.
	*/
	RESTRICTED static TerrainQuadTreeNode *const RESTRICT FindHighestNode(const Vector3 &position, TerrainQuadTree *const RESTRICT quadTree) NOEXCEPT
	{
		for (TerrainQuadTreeNode &node : quadTree->_RootNodes)
		{
			if (TerrainQuadTreeNode *const RESTRICT childNode{ FindHighestNode(position, &node) })
			{
				return childNode;
			}
		}

		return nullptr;
	}

	/*
	*	Given a position and a node, find the child node with the highest depth that the position is within.
	*/
	RESTRICTED static TerrainQuadTreeNode *const RESTRICT FindHighestNode(const Vector3 &position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
	{
		if (node->_Subdivided)
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (TerrainQuadTreeNode *const RESTRICT childNode{ FindHighestNode(position, &node->_ChildNodes[i]) })
				{
					return childNode;
				}
			}
		}

		if (node->IsWithin(position))
		{
			return node;
		}

		else
		{
			return nullptr;
		}
	}

};