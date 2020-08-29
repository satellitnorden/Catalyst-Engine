#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/Task.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainQuadTree.h>
#include <Terrain/TerrainRayTracingData.h>
#include <Terrain/TerrainUpdate.h>

class TerrainSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TerrainSystem);

	/*
	*	Default constructor.
	*/
	TerrainSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the terrain system.
	*/
	void Initialize(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT;

	/*
	*	Updates the terrain system during the sequential update phase.
	*/
	void SequentialUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the terrain properties.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Sets the terrain world center.
	*/
	void SetTerrainWorldCenter(const WorldPosition &world_position) NOEXCEPT;

	/*
	*	Sets the height map.
	*/
	void SetHeightMap(const Texture2D<float> &height_map) NOEXCEPT;

	/*
	*	Updates the height map.
	*/
	RESTRICTED NO_DISCARD Texture2D<float> *const RESTRICT UpdateHeightMap() NOEXCEPT;

	/*
	*	Sets the index map.
	*/
	void SetIndexMap(const Texture2D<Vector4<uint8>> &index_map) NOEXCEPT;

	/*
	*	Updates the index map.
	*/
	RESTRICTED NO_DISCARD Texture2D<Vector4<uint8>> *const RESTRICT UpdateIndexMap() NOEXCEPT;

	/*
	*	Sets the blend map.
	*/
	void SetBlendMap(const Texture2D<Vector4<uint8>> &blend_map) NOEXCEPT;

	/*
	*	Updates the blend map.
	*/
	RESTRICTED NO_DISCARD Texture2D<Vector4<uint8>> *const RESTRICT UpdateBlendMap() NOEXCEPT;

	/*
	*	Returns the terrain patch informations.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD DynamicArray<TerrainPatchInformation>* const RESTRICT GetTerrainPatchInformations() NOEXCEPT
	{
		return &_PatchInformations;
	}

	/*
	*	Returns the terrain patch render informations.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD DynamicArray<TerrainPatchRenderInformation>* const RESTRICT GetTerrainPatchRenderInformations() NOEXCEPT
	{
		return &_PatchRenderInformations;
	}

	/*
	*	Returns the terrain map coordinate at the given position.
	*/
	NO_DISCARD Vector2<float> GetTerrainMapCoordinateAtPosition(const Vector3<float> &position) const NOEXCEPT;

	/*
	*	Returns the terrain height at the given position.
	*/
	bool GetTerrainHeightAtPosition(const Vector3<float>& position, float* const RESTRICT height, const void* const RESTRICT context = nullptr) const NOEXCEPT;

	/*
	*	Returns the terrain normal at the given position.
	*	Can optionally retrieve the height at the same time.
	*/
	bool GetTerrainNormalAtPosition(const Vector3<float>& position, Vector3<float>* const RESTRICT normal, float* const RESTRICT height = nullptr, const void* const RESTRICT context = nullptr) const NOEXCEPT;

	/*
	*	Returns the terrain material at the given position.
	*/
	bool GetTerrainMaterialAtPosition(const Vector3<float> &position, Vector4<uint8> *const RESTRICT indices, Vector4<float> *const RESTRICT blend, const void *const RESTRICT context = nullptr) const NOEXCEPT;

private:

	//The properties.
	TerrainProperties _Properties;

	//The quad tree.
	TerrainQuadTree _QuadTree;

	//The update task.
	Task _UpdateTask;

	//The patch informations.
	DynamicArray<TerrainPatchInformation> _PatchInformations;

	//The patch render informations.
	DynamicArray<TerrainPatchRenderInformation> _PatchRenderInformations;

	//The update.
	TerrainUpdate _Update;

	//Denotes if the height map was updated.
	bool _HeightMapUpdated{ false };

	//Denotes if the index map was updated.
	bool _IndexMapUpdated{ false };

	//Denotes if the blend map was updated.
	bool _BlendMapUpdated{ false };

	//The terrain ray tracing data.
	TerrainRayTracingData _TerrainRayTracingData;

	/*
	*	Processes the update.
	*/
	void ProcessUpdate() NOEXCEPT;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateAsynchronous() NOEXCEPT;

	/*
	*	Removes a quad tree root node.
	*/
	void RemoveRootNode(const GridPoint2 grid_point) NOEXCEPT;

	/*
	*	Adds a quad tree root node.
	*/
	void AddRootNode(const GridPoint2 grid_point) NOEXCEPT;

	/*
	*	Removes a node.
	*/
	void RemoveNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Checks combination of a node. Returns whether or not the node was combined.
	*/
	bool CheckCombination(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Checks subdivisions of a node. Returns whether or not the node was subdivided.
	*/
	bool CheckSubdivision(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Combines a node.
	*/
	void CombineNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Subdivides a node.
	*/
	void SubdivideNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Traverses the quad tree, calculates new borders for all nodes and fills in the update.
	*/
	void CalculateNewBorders() NOEXCEPT;

	/*
	*	Calculates new borders for one node and fills in the update.
	*/
	void CalculateNewBorders(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Generates the patch informations.
	*/
	void GeneratePatchInformations(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Updates the terrain ray tracing data.
	*/
	void UpdateTerrainRayTracingData() NOEXCEPT;

};