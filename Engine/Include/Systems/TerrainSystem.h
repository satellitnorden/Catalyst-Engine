#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/GridPoint2.h>

//Multithreading.
#include <Multithreading/Spinlock.h>
#include <Multithreading/Task.h>

//Terrain.
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainQuadTree.h>
#include <Terrain/TerrainUpdate.h>

//Forward declarations.
class CatalystProjectTerrainConfiguration;

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
	void InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT;

	/*
	*	Updates the terrain system sequentially.
	*/
	void SequentialUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Returns the terrain properties, const.
	*/
	RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the terrain properties, non-const.
	*/
	RESTRICTED NO_DISCARD TerrainProperties *const RESTRICT GetTerrainProperties() NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the low detail terrain patch informations.
	*/
	RESTRICTED NO_DISCARD DynamicArray<TerrainPatchInformation> *const RESTRICT GetTerrainPatchInformations() NOEXCEPT
	{
		return &_PatchInformations;
	}

	/*
	*	Returns the low detail terrain patch render informations.
	*/
	RESTRICTED NO_DISCARD DynamicArray<TerrainPatchRenderInformation> *const RESTRICT GetTerrainPatchRenderInformations() NOEXCEPT
	{
		return &_PatchRenderInformations;
	}

	/*
	*	Returns the terrain height at the given position.
	*/
	bool GetTerrainHeightAtPosition(const Vector3 &position, float *const RESTRICT height) const NOEXCEPT;

	/*
	*	Returns the terrain normal at the given position.
	*/
	bool GetTerrainNormalAtPosition(const Vector3 &position, Vector3 *const RESTRICT normal) const NOEXCEPT;

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
	*	Processes the update.
	*/
	void ProcessUpdate() NOEXCEPT;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Checks restoration of a node. Returns whether or not the node was restored.
	*/
	bool CheckRestoration(const Vector3 &viewerPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Checks subdivisions of a node. Returns whether or not the node was subdivided.
	*/
	bool CheckSubdivision(const Vector3 &viewerPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Returns whether or not a node should be subdivided.
	*/
	bool ShouldBeSubdivided(const Vector3 &viewerPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Restores a node.
	*/
	void RestoreNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Subdivides a node.
	*/
	void SubdivideNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

	/*
	*	Generates a patch.
	*/
	void GeneratePatch(const Vector3 &worldPosition, const float patchSizeMultiplier, const uint8 normalResolutionMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;

};