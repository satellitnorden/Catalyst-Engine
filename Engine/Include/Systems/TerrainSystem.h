#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/Task.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/TerrainHeightGenerationComputePipeline.h>
#include <Rendering/Native/Pipelines/ComputePipelines/TerrainMaterialsGenerationComputePipeline.h>

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
	*	Post-initializes the terrain system.
	*/
	void PostInitialize(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT;

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
	*	Sets the maximum quad tree depth.
	*/
	void SetMaximumQuadTreeDepth(const uint8 value) NOEXCEPT;

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
	NO_DISCARD Vector2<float32> GetTerrainMapCoordinateAtPosition(const Vector3<float32> &position) const NOEXCEPT;

	/*
	*	Returns the terrain height at the given position.
	*/
	bool GetTerrainHeightAtPosition(const Vector3<float32> &position, float32 *const RESTRICT height, const void *const RESTRICT context = nullptr) const NOEXCEPT;

	/*
	*	Returns the terrain normal at the given position.
	*	Can optionally retrieve the height at the same time.
	*/
	bool GetTerrainNormalAtPosition(const Vector3<float32>& position, Vector3<float32> *const RESTRICT normal, float32 *const RESTRICT height = nullptr, const void *const RESTRICT context = nullptr) const NOEXCEPT;

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

	//The terrain ray tracing data.
	TerrainRayTracingData _TerrainRayTracingData;

	//Denotes if terrain generation is running.
	bool _TerrainGenerationRunning{ false };

	//The commandbuffer.
	CommandBuffer *RESTRICT _CommandBuffer{ nullptr };

	//The terrain generation event.
	EventHandle _TerrainGenerationEvent;

	//The terrain height generation compute pipeline.
	TerrainHeightGenerationComputePipeline _TerrainHeightGenerationComputePipeline;

	//The terrain materials generation compute pipeline.
	TerrainMaterialsGenerationComputePipeline _TerrainMaterialsGenerationComputePipeline;

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
	*	Checks combination of a node.
	*/
	void CheckCombination(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Checks subdivisions of a node.
	*/
	void CheckSubdivision(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Combines a node.
	*/
	void CombineNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Subdivides a node.
	*/
	void SubdivideNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Finds the highest depth.
	*/
	void FindHighestDepth(const TerrainQuadTreeNode &node, uint8 *const RESTRICT highest_depth) NOEXCEPT;

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

	/*
	*	Generates the maps for the given node.
	*/
	void GenerateMaps(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Destroys the maps for the given node.
	*/
	void DestroyMaps(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Finishes terrain generation.
	*/
	void FinishTerrainGeneration() NOEXCEPT;

};