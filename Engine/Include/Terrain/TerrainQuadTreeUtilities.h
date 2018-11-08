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

};