#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Pointers/UniquePointer.h>

//Terrain.
#include <Terrain/TerrainPatch.h>
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainRenderInformation.h>

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
	TerrainSystem() NOEXCEPT { }

	/*
	*	Initializes the terrain system.
	*/
	void InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT;

	/*
	*	Returns the terrain properties.
	*/
	RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the terrain render information.
	*/
	RESTRICTED NO_DISCARD const DynamicArray<TerrainRenderInformation> *const RESTRICT GetTerrainRenderInformation() const NOEXCEPT
	{
		return &_RenderInformation;
	}

	/*
	*	Adds a terrain patch.
	*/
	void AddTerrainPatch(TerrainPatchInformation &&information) NOEXCEPT;

	/*
	*	Returns the terrain height at the given position.
	*/
	float GetTerrainHeightAtPosition(const Vector3 &position) const NOEXCEPT;

	/*
	*	Returns the terrain normal at the given position.
	*/
	Vector3 GetTerrainNormalAtPosition(const Vector3 &position) const NOEXCEPT;

private:

	//The properties.
	TerrainProperties _Properties;

	//The patches.
	DynamicArray<TerrainPatch> _Patches;

	//The render information.
	DynamicArray<TerrainRenderInformation> _RenderInformation;

};