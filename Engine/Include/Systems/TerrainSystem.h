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
#include <Terrain/TerrainProcessedUpdate.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainQuadTree.h>
#include <Terrain/TerrainQuadTreeNodeUpdate.h>
#include <Terrain/TerrainRayTracingData.h>
#include <Terrain/TerrainUpdate.h>

class ALIGN(8) TerrainSystem final
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
	void SequentialUpdate() NOEXCEPT;

	/*
	*	Returns the terrain properties.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns if terrain can be generated.
	*/
	NO_DISCARD bool CanTerrainBeGenerated() const NOEXCEPT;

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
	*	Returns the maximum height.
	*/
	bool GetMaximumHeight(float32 *const RESTRICT maximum_height) const NOEXCEPT;

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

	//Enumeration covering all update stages.
	enum class UpdateStage : uint8
	{
		GENERATE_UPDATES,
		WAIT_FOR_UPDATES,
		PROCESS_UPDATES
	};

	//The properties.
	TerrainProperties _Properties;

	//The quad tree.
	TerrainQuadTree _QuadTree;

	//The patch informations.
	DynamicArray<TerrainPatchInformation> _PatchInformations;

	//The patch render informations.
	DynamicArray<TerrainPatchRenderInformation> _PatchRenderInformations;

	//The current update stage.
	UpdateStage _CurrentUpdateStage{ UpdateStage::GENERATE_UPDATES };

	//The updates.
	DynamicArray<TerrainQuadTreeNodeUpdate *RESTRICT> _Updates;

	//The process updates task.
	Task _ProcessUpdatesTask;

	//The processed update.
	TerrainProcessedUpdate _ProcessedUpdate;

	//The current number of updates in flight.
	uint32 _CurrentNumberOfUpdatesInFlight;

	//The maximum number of updates in flight.
	uint32 _MaximumNumberOfUpdatesInFlight;

	//The terrain ray tracing data.
	TerrainRayTracingData _TerrainRayTracingData;

	/*
	*	Updates the GENERATE_UPDATES stage.
	*/
	void UpdateGenerateUpdatesStage() NOEXCEPT;

	/*
	*	Updates the WAIT_FOR_UPDATES stage.
	*/
	void UpdateWaitForUpdatesStage() NOEXCEPT;

	/*
	*	Updates the PROCESS_UPDATES stage.
	*/
	void UpdateProcessUpdatesStage() NOEXCEPT;

	/*
	*	Removes a node.
	*/
	void RemoveNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Checks combination of a node.
	*/
	void CheckCombination(const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Checks subdivisions of a node.
	*/
	void CheckSubdivision(const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Traverses the quad tree, calculates new borders for all nodes and fills in the update.
	*/
	void CalculateNewBorders() NOEXCEPT;

	/*
	*	Calculates new borders for one node and fills in the update.
	*/
	void CalculateNewBorders(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Gathers patch informations.
	*/
	void GatherPatchInformations(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

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
	*	Performs the given update.
	*/
	void PerformUpdate(TerrainQuadTreeNodeUpdate *const RESTRICT update) NOEXCEPT;

	/*
	*	Processes updates asynchronously.
	*/
	void ProcessUpdatesAsynchronously() NOEXCEPT;

	/*
	*	Saves the terrain data.
	*/
	void SaveTerrainData() NOEXCEPT;

	/*
	*	Loads the terrain data.
	*/
	void LoadTerrainData() NOEXCEPT;

};