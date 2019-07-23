#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Multithreading.
#include <Multithreading/Spinlock.h>
#include <Multithreading/Task.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainQuadTree.h>
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
	void Initialize() NOEXCEPT;

	/*
	*	Updates the terrain system during the sequential update phase.
	*/
	void SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers the height function.
	*/
	FORCE_INLINE void RegisterHeightFunction(const HeightFunction function) NOEXCEPT
	{
		_Properties._HeightFunction = function;
	}

	/*
	*	Registers the material function.
	*/
	FORCE_INLINE void RegisterMaterialFunction(const MaterialFunction function) NOEXCEPT
	{
		_Properties._MaterialFunction = function;
	}

	/*
	*	Returns the terrain properties, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the terrain properties, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TerrainProperties *const RESTRICT GetTerrainProperties() NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the terrain patch informations.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD DynamicArray<TerrainPatchInformation> *const RESTRICT GetTerrainPatchInformations() NOEXCEPT
	{
		return &_PatchInformations;
	}

	/*
	*	Returns the terrain patch render informations.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD DynamicArray<TerrainPatchRenderInformation> *const RESTRICT GetTerrainPatchRenderInformations() NOEXCEPT
	{
		return &_PatchRenderInformations;
	}

	/*
	*	Returns the terrain height at the given position.
	*/
	bool GetTerrainHeightAtPosition(const Vector3<float> &position, float *const RESTRICT height) const NOEXCEPT;

	/*
	*	Returns the terrain normal at the given position.
	*	Can optionally retrieve the height at the same time.
	*/
	bool GetTerrainNormalAtPosition(const Vector3<float> &position, Vector3<float> *const RESTRICT normal, float *const RESTRICT height = nullptr) const NOEXCEPT;

	/*
	*	Returns the terrain material at the given position.
	*	Can optionally retrieve the height and the normal at the same time.
	*/
	bool GetTerrainMaterialAtPosition(const Vector3<float> &position, uint8 *const RESTRICT material, float *const RESTRICT height = nullptr, Vector3<float> *const RESTRICT normal = nullptr) const NOEXCEPT;

private:

	//The properties.
	TerrainProperties _Properties;

	//The quad tree.
	TerrainQuadTree _QuadTree;

	//The update.
	TerrainUpdate _Update;

	//The update task.
	Task _UpdateTask;

	//The patch informations.
	DynamicArray<TerrainPatchInformation> _PatchInformations;

	//The patch render informations.
	DynamicArray<TerrainPatchRenderInformation> _PatchRenderInformations;

	/*
	*	Returns the patch information index for the identifier.
	*/
	uint64 GetPatchInformationIndex(const uint64 identifier) const NOEXCEPT;

	/*
	*	Processes the update.
	*/
	void ProcessUpdate() NOEXCEPT;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateAsynchronous() NOEXCEPT;

	/*
	*	Removes a quad tree node.
	*/
	void RemoveNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Checks combination of a node. Returns whether or not the node was combined.
	*/
	bool CheckCombination(const uint8 depth, const Vector3<float> &perceiverPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Checks subdivisions of a node. Returns whether or not the node was subdivided.
	*/
	bool CheckSubdivision(const uint8 depth, const Vector3<float> &perceiverPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Combines a node.
	*/
	void CombineNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Subdivides a node.
	*/
	void SubdivideNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Generates a patch.
	*/
	void GeneratePatch(const Vector3<float> &worldPosition, const float patchSizeMultiplier, const uint8 depth, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;

	/*
	*	Destroys a patch.
	*/
	void DestroyPatch(const uint64 index) NOEXCEPT;

	/*
	*	Traverses the quad tree, calculates new borders for all nodes and fills in the update.
	*/
	void CalculateNewborders() NOEXCEPT;

	/*
	*	Calculates new borders for one node and fills in the update.
	*/
	void CalculateNewborders(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

};